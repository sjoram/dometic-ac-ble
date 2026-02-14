#pragma once
#include "esphome.h"
#include "esphome/components/ble_client/ble_client.h"

namespace esphome {
namespace dometic_ble {

class DometicBLE : public Component, public ble_client::BLEClientNode {
 public:
  void setup() override;
  void loop() override;
  void gattc_event_handler(
      esp_gattc_cb_event_t event,
      esp_gatt_if_t gattc_if,
      esp_ble_gattc_cb_param_t *param) override;

  void set_poll_interval(uint32_t ms) { poll_interval_ = ms; }

 protected:
  void send_command(std::vector<uint8_t> data);
  void poll_register(uint8_t reg);

  uint32_t poll_interval_{10000};
  uint32_t last_poll_{0};

  static const uint16_t SERVICE_UUID = 0x0400;
};

}  // namespace dometic_ble
}  // namespace esphome
