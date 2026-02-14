from esphome.components import switch
import esphome.codegen as cg
import esphome.config_validation as cv
from . import DometicBLE
from .const import CONF_SLEEP, CONF_LIGHT

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(DometicBLE),

        cv.Optional(CONF_SLEEP):
            switch.switch_schema(),

        cv.Optional(CONF_LIGHT):
            switch.switch_schema(),
    }
)

async def to_code(config):
    var = await cg.get_variable(config[cg.CONF_ID])

    if CONF_SLEEP in config:
        sw = await switch.new_switch(config[CONF_SLEEP])
        cg.add(var.sleep_switch = sw)

    if CONF_LIGHT in config:
        sw = await switch.new_switch(config[CONF_LIGHT])
        cg.add(var.light_switch = sw)
