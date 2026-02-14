#include "dometic_ble.h"

namespace esphome {
namespace dometic_ble {

static const char *TAG = "dometic_ble";

void DometicBLE::setup() {
  ESP_LOGI(TAG, "Dometic BLE component starting");
}

void DometicBLE::loop() {
  // optional polling logic
}
