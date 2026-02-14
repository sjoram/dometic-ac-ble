import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import ble_client
from esphome.const import CONF_ID

CODEOWNERS = ["@yourgithub"]

dometic_ns = cg.esphome_ns.namespace("dometic_ble")
DometicBLE = dometic_ns.class_("DometicBLE", cg.Component, ble_client.BLEClientNode)

CONF_BLE_CLIENT_ID = "ble_client_id"
CONF_POLL_INTERVAL = "poll_interval"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DometicBLE),
        cv.Required(CONF_BLE_CLIENT_ID): cv.use_id(ble_client.BLEClient),
        cv.Optional(CONF_POLL_INTERVAL, default="10s"): cv.positive_time_period_milliseconds,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_BLE_CLIENT_ID])
    cg.add(var.set_parent(parent))

    cg.add(var.set_poll_interval(config[CONF_POLL_INTERVAL]))
