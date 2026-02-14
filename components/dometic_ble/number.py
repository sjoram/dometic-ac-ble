from esphome.components import number
import esphome.codegen as cg
import esphome.config_validation as cv
from . import DometicBLE
from .const import CONF_TARGET_TEMPERATURE

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(DometicBLE),
        cv.Optional(CONF_TARGET_TEMPERATURE):
            number.number_schema(min_value=16, max_value=30, step=0.5),
    }
)

async def to_code(config):
    var = await cg.get_variable(config[cg.CONF_ID])

    if CONF_TARGET_TEMPERATURE in config:
        num = await number.new_number(config[CONF_TARGET_TEMPERATURE])
        cg.add(var.target_number = num)
