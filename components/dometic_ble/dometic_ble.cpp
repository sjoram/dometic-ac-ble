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
    esp_gatt_if_t gattc_if,
    esp_ble_gattc_cb_param_t *param) {

  switch (event) {

    case ESP_GATTC_OPEN_EVT:
      ESP_LOGI(TAG, "Connected to Dometic AC");
      break;

    case ESP_GATTC_SEARCH_CMPL_EVT: {
      ESP_LOGI(TAG, "Service discovery complete");

      for (auto &service : this->parent()->services()) {

        if (service.uuid.to_string() == SERVICE_UUID) {

          for (auto &chr : service.characteristics) {

            std::string uuid = chr.uuid.to_string();

            if (uuid == CHAR_WRITE_UUID) {
              write_handle_ = chr.handle;
              ESP_LOGI(TAG, "Found write characteristic");
            }

            if (uuid == CHAR_NOTIFY_UUID) {
              notify_handle_ = chr.handle;
              this->parent()->register_for_notify(notify_handle_);
              ESP_LOGI(TAG, "Found notify characteristic");
            }
          }
        }
