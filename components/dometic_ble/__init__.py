from esphome.components import ble_client
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_MAC_ADDRESS

DEPENDENCIES = ["ble_client"]
AUTO_LOAD = ["sensor", "number", "select"]

dometic_ns = cg.esphome_ns.namespace("dometic_ble")
DometicBLE = dometic_ns.class_("DometicBLE", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DometicBLE),
    cv.Required(CONF_MAC_ADDRESS): cv.mac_address,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_mac(config[CONF_MAC_ADDRESS]))
