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

CONF_NORTH_CORRECTION = "north_correction"
CONF_PRECIPITATION_INTENSITY = "precipitation_intensity"
CONF_SECONDARY_INTERCARDINAL_DIRECTION = "secondary_intercardinal_direction"
CONF_WEATHER_CONDITIONS = "weather_conditions"
CONF_WIND_DIRECTION = "wind_direction"
ICON_WEATHER_PARTLY_CLOUDY = "mdi:weather-partly-cloudy"
ICON_WEATHER_POURING = "mdi:weather-pouring"
ICON_WEATHER_SUNNY = "mdi:weather-sunny"

TYPES = [
    CONF_WIND_SPEED,
    CONF_WIND_DIRECTION,
    CONF_LIGHT,
    CONF_PRECIPITATION_INTENSITY,
    CONF_WEATHER_CONDITIONS,
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
            ).extend({
                cv.Optional(CONF_SECONDARY_INTERCARDINAL_DIRECTION): cv.boolean,
                cv.Optional(CONF_NORTH_CORRECTION): cv.int_range(min=-180, max=180),
            }),
            cv.Optional(CONF_LIGHT): text_sensor.text_sensor_schema(
                icon=ICON_WEATHER_SUNNY,
            ),
            cv.Optional(CONF_PRECIPITATION_INTENSITY): text_sensor.text_sensor_schema(
                icon=ICON_WEATHER_POURING,
            ),
            cv.Optional(CONF_WEATHER_CONDITIONS): text_sensor.text_sensor_schema(
                icon=ICON_WEATHER_PARTLY_CLOUDY,
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
    if conf := config.get(CONF_WIND_DIRECTION):
        if CONF_NORTH_CORRECTION in conf:
            cg.add(paren.set_north_correction(conf[CONF_NORTH_CORRECTION]))
        if CONF_SECONDARY_INTERCARDINAL_DIRECTION in conf:
            cg.add(paren.set_secondary_intercardinal_direction(conf[CONF_SECONDARY_INTERCARDINAL_DIRECTION]))
