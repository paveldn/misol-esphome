import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_BATTERY_LEVEL,
    CONF_THRESHOLD,
    DEVICE_CLASS_BATTERY,
    ENTITY_CATEGORY_DIAGNOSTIC,
)
from . import (
    CONF_MISOL_ID,
    WeatherStation,
)

CODEOWNERS = ["@paveldn"]

CONF_NIGHT = "night"
CONF_LOWER = "lower"
CONF_UPPER = "upper"
ICON_WEATHER_NIGHT = "mdi:weather-night"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_MISOL_ID): cv.use_id(WeatherStation),
            cv.Optional(CONF_BATTERY_LEVEL): binary_sensor.binary_sensor_schema(
                entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
                device_class=DEVICE_CLASS_BATTERY,
            ),
            cv.Optional(CONF_NIGHT): binary_sensor.binary_sensor_schema(
                icon=ICON_WEATHER_NIGHT,
            ).extend(
                {
                    cv.Optional(
                        CONF_THRESHOLD, 
                        default={
                            CONF_UPPER: 5.5,
                            CONF_LOWER: 4.5,
                        },
                    ): cv.Any(
                        cv.float_,
                        cv.Schema(
                            {
                                cv.Required(CONF_UPPER): cv.float_,
                                cv.Required(CONF_LOWER): cv.float_,
                            }
                        ),
                    ),
                }
            ),
        }
    ),
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_MISOL_ID])

    if conf := config.get(CONF_BATTERY_LEVEL):
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(paren.set_battery_level_binary_sensor(sens))
    if conf := config.get(CONF_NIGHT):
        sens = await binary_sensor.new_binary_sensor(conf)
        cg.add(paren.set_night_binary_sensor(sens))
        if threshold := conf.get(CONF_THRESHOLD):
            if isinstance(threshold, float):
                cg.add(paren.set_upper_night_threshold(threshold))
                cg.add(paren.set_lower_night_threshold(threshold))
            else:
                cg.add(paren.set_upper_night_threshold(threshold[CONF_UPPER]))
                cg.add(paren.set_lower_night_threshold(threshold[CONF_LOWER]))
