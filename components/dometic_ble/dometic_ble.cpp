#include "dometic_ble.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dometic_ble {

static const char *TAG = "dometic_ble";

void DometicBLE::setup() {
  ESP_LOGI(TAG, "Setting up Dometic BLE client...");
}

void DometicBLE::loop() {

  if (!this->parent()->is_connected())
    return;

  // Handle queued BLE writes
  if (!write_queue_.empty() && !write_in_progress_) {
    send_next_command_();
  }

  // Poll scheduler
  if (millis() - last_poll_ > poll_interval_) {
    last_poll_ = millis();

    switch (poll_cycle_) {
      case 0: poll_register_(10); break; // actual temp
      case 1: poll_register_(4); break;  // target temp
      case 2: poll_register_(3); break;  // mode
      case 3: poll_register_(2); break;  // fan
      case 4: poll_register_(27); break; // sleep
    }

    poll_cycle_ = (poll_cycle_ + 1) % 5;
  }
}

void DometicBLE::queue_command_(std::vector<uint8_t> cmd) {
  write_queue_.push(cmd);
}

void DometicBLE::send_next_command_() {
  if (write_queue_.empty()) return;

  auto cmd = write_queue_.front();
  write_queue_.pop();

  write_in_progress_ = true;

  this->parent()->write_gatt_char(
      SERVICE_UUID,
      CHAR_WRITE_UUID,
      cmd
  );

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

void DometicBLE::process_packet_(const uint8_t *data, uint16_t length) {
  if (length < 7) return;

  uint8_t reg = data[1];

  switch (reg) {

    case 10: { // actual temperature
      float temp = decode_temp_(data[5], data[6]);
      if (actual_temp_sensor)
        actual_temp_sensor->publish_state(temp);
      break;
    }

    case 4: { // target temperature
      float temp = decode_temp_(data[5], data[6]);
      if (target_temp_sensor)
        target_temp_sensor->publish_state(temp);
      break;
    }

    case 3: { // mode
      uint8_t mode = data[5];
      if (mode_select)
        mode_select->publish_state(std::to_string(mode));
      break;
    }

    case 2: { // fan
      uint8_t fan = data[5];
      if (fan_select)
        fan_select->publish_state(std::to_string(fan));
      break;
    }

    case 1: { // compressor
      bool running = data[5] & 0x01;
      if (compressor_sensor)
        compressor_sensor->publish_state(running);
      break;
    }

    case 27: { // sleep
      bool sleep = data[5];
      if (sleep_switch)
        sleep_switch->publish_state(sleep);
      break;
    }

    case 28: { // light
      bool light = data[5];
      if (light_switch)
        light_switch->publish_state(light);
      break;
    }

/*    case 0x1C: { // error/status
      uint8_t err = data[5];
      if (!status_text) break;
*/
      switch (err) {
        case 0: status_text->publish_state("OK"); break;
        case 1: status_text->publish_state("Low Voltage"); break;
        case 2: status_text->publish_state("Sensor Fault"); break;
        default: status_text->publish_state("Unknown Error");
      }
      break;
    }

    default:
      break;
  }
}

void DometicBLE::set_target_temperature(float temp) {
  int16_t raw = temp * 1000;
  std::vector<uint8_t> cmd = {
    0xAA, 4, 0x02,
    (uint8_t)(raw & 0xFF),
    (uint8_t)((raw >> 8) & 0xFF)
  };
  queue_command_(cmd);
}

void DometicBLE::set_mode(uint8_t mode) {
  std::vector<uint8_t> cmd = {0xAA, 3, 0x01, mode};
  queue_command_(cmd);
}

void DometicBLE::set_fan(uint8_t fan) {
  std::vector<uint8_t> cmd = {0xAA, 2, 0x01, fan};
  queue_command_(cmd);
}

void DometicBLE::set_sleep(bool state) {
  std::vector<uint8_t> cmd = {0xAA, 27, 0x01, state};
  queue_command_(cmd);
}

void DometicBLE::set_light(bool state) {
  std::vector<uint8_t> cmd = {0xAA, 28, 0x01, state};
  queue_command_(cmd);
}

}  // namespace dometic_ble
}  // namespace esphome
