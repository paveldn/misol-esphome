import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
  CONF_LIGHT,
  CONF_WIND_SPEED,
  ICON_SIGN_DIRECTION,
  ICON_WEATHER_WINDY,
)
from . import (
    CONF_MISOL_ID,
    WeatherStation,
)

CODEOWNERS = ["@paveldn"]

CONF_WIND_DIRECTION = "wind_direction"
ICON_WEATHER_SUNNY = "mdi:weather-sunny"


TYPES = [ 
    CONF_WIND_SPEED,
    CONF_WIND_DIRECTION,
    CONF_LIGHT,
]

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_MISOL_ID): cv.use_id(WeatherStation),
            cv.Optional(CONF_WIND_SPEED): text_sensor.text_sensor_schema(
                icon=ICON_WEATHER_WINDY,
            ),
            cv.Optional(CONF_WIND_DIRECTION): text_sensor.text_sensor_schema(
                icon=ICON_SIGN_DIRECTION,
            ),
            cv.Optional(CONF_LIGHT): text_sensor.text_sensor_schema(
                icon=ICON_WEATHER_SUNNY,
            ),
            
        }
    ),
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_MISOL_ID])

    for key in TYPES:
        if sensor_config := config.get(key):
            sens = await text_sensor.new_text_sensor(sensor_config)
            cg.add(getattr(paren, f"set_{key}_text_sensor")(sens))
