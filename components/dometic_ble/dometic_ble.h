#pragma once

#include "esphome.h"
#include "esphome/components/ble_client/ble_client.h"

namespace esphome {
namespace dometic_ble {

class DometicBLE : public Component, public ble_client::BLEClientNode {
 public:
  void setup() override;
  void loop() override;

  void set_mac(uint64_t mac) { mac_ = mac; }

 protected:
  uint64_t mac_;
};

}  // namespace dometic_ble
}  // namespace esphome
