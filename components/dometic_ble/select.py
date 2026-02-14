from esphome.components import select
import esphome.codegen as cg
import esphome.config_validation as cv
from . import DometicBLE
from .const import CONF_MODE, CONF_FAN

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(DometicBLE),

        cv.Optional(CONF_MODE):
            select.select_schema(options=["0","1","2","3","4"]),

        cv.Optional(CONF_FAN):
            select.select_schema(options=["1","2","3","4","5"]),
    }
)

async def to_code(config):
    var = await cg.get_variable(config[cg.CONF_ID])

    if CONF_MODE in config:
        sel = await select.new_select(config[CONF_MODE])
        cg.add(var.mode_select = sel)

    if CONF_FAN in config:
        sel = await select.new_select(config[CONF_FAN])
        cg.add(var.fan_select = sel)
