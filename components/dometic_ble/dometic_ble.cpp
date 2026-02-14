#include "dometic_ble.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dometic_ble {

static const char *TAG = "dometic_ble";

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
      case 0: poll_register_(10); break; // actual temp
      case 1: poll_register_(4);  break; // target temp
      case 2: poll_register_(3);  break; // mode
      case 3: poll_register_(2);  break; // fan
      case 4: poll_register_(27); break; // sleep
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

    case ESP_GATTC_NOTIFY_EVT:
      process_packet_(param->notify.value,
                      param->notify.value_len);
      break;

    case ESP_GATTC_DISCONNECT_EVT:
      ESP_LOGW(TAG, "Disconnected from Dometic AC");
      break;

    default:
      break;
  }
}

void DometicBLE::queue_command_(std::vector<uint8_t> cmd) {
  write_queue_.push(cmd);
}

void DometicBLE::send_next_command_() {
  if (write_queue_.empty())
    return;

  auto cmd = write_queue_.front();
  write_queue_.pop();

  write_in_progress_ = true;

  // Modern ESPHome BLEClientNode write API
  this->write(
      SERVICE_UUID,
      CHAR_WRITE_UUID,
      cmd,
      false   // change to true if AC requires write-with-response
  );

  write_in_progress_ = false;
}

void DometicBLE::poll_register_(uint8_t reg) {
  std::vector<uint8_t> cmd = {0xA_
