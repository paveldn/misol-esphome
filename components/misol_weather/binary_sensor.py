import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_BATTERY_LEVEL,
    DEVICE_CLASS_BATTERY,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import (
    CONF_MISOL_ID,
    WeatherStation,
)

CODEOWNERS = ["@paveldn"]

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_MISOL_ID): cv.use_id(WeatherStation),
            cv.Optional(CONF_BATTERY_LEVEL): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                device_class=DEVICE_CLASS_BATTERY,
            ),
        }
    ),
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_MISOL_ID])

    if conf := config.get(CONF_BATTERY_LEVEL):
        bat_sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(paren.set_battery_level_binary_sensor(bat_sens))
