#include "dometic_ble.h"
#include "esphome/core/log.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"

namespace esphome {
namespace dometic_ble {

static const char *TAG = "dometic_ble";

static const esp32_ble_tracker::ESPBTUUID service_uuid =
    esp32_ble_tracker::ESPBTUUID::from_raw(SERVICE_UUID);

static const esp32_ble_tracker::ESPBTUUID write_uuid =
    esp32_ble_tracker::ESPBTUUID::from_raw(CHAR_WRITE_UUID);

static const esp32_ble_tracker::ESPBTUUID notify_uuid =
    esp32_ble_tracker::ESPBTUUID::from_raw(CHAR_NOTIFY_UUID);

void DometicBLE::setup() {
  ESP_LOGI(TAG, "Setting up Dometic BLE client...");
}

void DometicBLE::loop() {
  if (!this->parent()->connected())
    return;

  if (!write_queue_.empty() && !write_in_progress_) {
    send_next_command_();
  }

  if (millis() - last_poll_ > poll_interval_) {
    last_poll_ = millis();

    switch (poll_cycle_) {
      case 0: poll_register_(10); break;
      case 1: poll_register_(4);  break;
      case 2: poll_register_(3);  break;
      case 3: poll_register_(2);  break;
      case 4: poll_register_(27); break;
    }

    poll_cycle_ = (poll_cycle_ + 1) % 5;
  }
}

void DometicBLE::gattc_event_handler(
    esp_gattc_cb_event_t event,
    esp_gatt_if_t,
    esp_ble_gattc_cb_param_t *param) {

  switch (event) {

    case ESP_GATTC_OPEN_EVT:
      ESP_LOGI(TAG, "Connected to Dometic AC");
      break;

    case ESP_GATTC_SEARCH_CMPL_EVT: {
      ESP_LOGI(TAG, "Service discovery complete");

      auto *chr = this->parent()->get_characteristic(service_uuid, write_uuid);
      if (chr != nullptr) {
        write_handle_ = chr->handle;
        ESP_LOGI(TAG, "Write characteristic found");
      }

      auto *notify_chr =
          this->parent()->get_characteristic(service_uuid, notify_uuid);
      if (notify_chr != nullptr) {
        notify_handle_ = notify_chr->handle;
        this->parent()->register_for_notify(notify_handle_);
        ESP_LOGI(TAG, "Notify characteristic found");
      }

      break;
    }

    case ESP_GATTC_NOTIFY_EVT:
      process_packet_(param->notify.value,
                      param->notify.value_len);
      break;

    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGW(TAG, "Disconnected");
      write_handle_ = 0;
      notify_handle_ = 0;
      break;

    default:
      break;
  }
}

void DometicBLE::queue_command_(std::vector<uint8_t> cmd) {
  write_queue_.push(cmd);
}

void DometicBLE::send_next_command_() {
  if (write_queue_.empty() || write_handle_ == 0)
    return;

  auto cmd = write_queue_.front();
  write_queue_.pop();

  write_in_progress_ = true;

  this->parent()->write(
      write_handle_,
      cmd.data(),
      cmd.size(),
      false);

  write_in_progress_ = false;
}

void DometicBLE::poll_register_(uint8_t reg) {
  std::vector<uint8_t> cmd = {0xAA, reg, 0x00, 0x00};
  queue_command_(cmd);
}

float DometicBLE::decode_temp_(uint8_t low, uint8_t high) {
  int16_t raw = (int16_t)(low | (high << 8));
  return raw / 1000.0f;
}

