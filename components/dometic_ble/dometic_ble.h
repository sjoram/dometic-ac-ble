#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/select/select.h"
#include "esphome/components/number/number.h"
#include "esphome/components/switch/switch.h"

#include <queue>

namespace esphome {
namespace dometic_ble {

static const char *SERVICE_UUID = "0000fff0-0000-1000-8000-00805f9b34fb";
static const char *CHAR_NOTIFY_UUID = "0000fff1-0000-1000-8000-00805f9b34fb";
static const char *CHAR_WRITE_UUID = "0000fff2-0000-1000-8000-00805f9b34fb";

enum DometicMode {
  MODE_COOL = 0,
  MODE_HEAT = 1,
  MODE_FAN = 2,
  MODE_AUTO = 3,
  MODE_DRY = 4,
};

enum DometicFan {
  FAN_LOW = 1,
  FAN_MEDIUM = 2,
  FAN_HIGH = 3,
  FAN_TURBO = 4,
  FAN_AUTO = 5,
};

class DometicBLE : public Component, public ble_client::BLEClientNode {
 public:
  void setup() override;
  void loop() override;

  void set_target_temperature(float temp);
  void set_mode(uint8_t mode);
  void set_fan(uint8_t fan);
  void set_sleep(bool state);
  void set_light(bool state);

  void set_poll_interval(uint32_t interval) { poll_interval_ = interval; }

  // Entity setters (connected via python side)
  sensor::Sensor *actual_temp_sensor{nullptr};
  sensor::Sensor *target_temp_sensor{nullptr};
  sensor::Sensor *compressor_sensor{nullptr};
  text_sensor::TextSensor *status_text{nullptr};
  select::Select *mode_select{nullptr};
  select::Select *fan_select{nullptr};
  number::Number *target_number{nullptr};
  switch_::Switch *sleep_switch{nullptr};
  switch_::Switch *light_switch{nullptr};

 protected:
  void process_packet_(const uint8_t *data, uint16_t length);
  void queue_command_(std::vector<uint8_t> cmd);
  void send_next_command_();
  void poll_register_(uint8_t reg);
  float decode_temp_(uint8_t low, uint8_t high);

  std::queue<std::vector<uint8_t>> write_queue_;
  bool write_in_progress_{false};

  uint32_t last_poll_{0};
  uint32_t poll_interval_{10000};
  uint8_t poll_cycle_{0};
};

}  // namespace dometic_ble
}  // namespace esphome
