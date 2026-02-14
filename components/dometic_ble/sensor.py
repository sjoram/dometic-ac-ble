import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from . import dometic_ns, DometicBLE
from .const import *

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(DometicBLE),

        cv.Optional(CONF_ACTUAL_TEMPERATURE):
            sensor.sensor_schema(unit_of_measurement="°C"),

        cv.Optional(CONF_TARGET_TEMPERATURE):
            sensor.sensor_schema(unit_of_measurement="°C"),

        cv.Optional(CONF_COMPRESSOR):
            sensor.sensor_schema(),
    }
)

async def to_code(config):
    var = await cg.get_variable(config[cg.CONF_ID])

    if CONF_ACTUAL_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_ACTUAL_TEMPERATURE])
        cg.add(var.actual_temp_sensor = sens)

    if CONF_TARGET_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TARGET_TEMPERATURE])
        cg.add(var.target_temp_sensor = sens)

    if CONF_COMPRESSOR in config:
        sens = await sensor.new_sensor(config[CONF_COMPRESSOR])
        cg.add(var.compressor_sensor = sens)
