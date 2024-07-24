import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_HUMIDITY,
    CONF_LIGHT,
    CONF_PRESSURE,
    CONF_TEMPERATURE,
    CONF_WIND_DIRECTION_DEGREES,
    CONF_WIND_SPEED,
    DEVICE_CLASS_ATMOSPHERIC_PRESSURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_ILLUMINANCE,
    DEVICE_CLASS_PRECIPITATION,
    DEVICE_CLASS_PRECIPITATION_INTENSITY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_WIND_SPEED,
    ICON_SIGN_DIRECTION,
    ICON_WEATHER_WINDY,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_NONE,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CELSIUS,
    UNIT_DEGREES,
    UNIT_HECTOPASCAL,
    UNIT_LUX,
    UNIT_PERCENT,
)
from . import (
    CONF_MISOL_ID,
    WeatherStation,
)

CODEOWNERS = ["@paveldn"]

CONF_ACCUMULATED_PRECIPITATION = "accumulated_precipitation"
CONF_PRECIPITATION_INTENSITY = "precipitation_intensity"
CONF_UV_INDEX = "uv_index"
CONF_UV_INTENSITY = "uv_intensity"
CONF_WIND_GUST = "wind_gust"
ICON_SUN_WIRELESS = "mdi:sun-wireless-outline"
UNIT_METER_PER_SECOND = "m/s"
UNIT_MILLIMETERS = "mm"
UNIT_MILLIMETERS_PER_HOUR = "mm/h"
UNIT_ULTRAVIOLET_INTENSITY = "mW/m²"

TYPES = [
    CONF_TEMPERATURE,
    CONF_HUMIDITY,
    CONF_PRESSURE,
    CONF_WIND_SPEED,
    CONF_WIND_DIRECTION_DEGREES,
    CONF_WIND_GUST,
    CONF_ACCUMULATED_PRECIPITATION,
    CONF_PRECIPITATION_INTENSITY,
    CONF_LIGHT,
    CONF_UV_INTENSITY,
    CONF_UV_INDEX,
]

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(CONF_MISOL_ID): cv.use_id(WeatherStation),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_CELSIUS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_TEMPERATURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_HUMIDITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_PERCENT,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_HUMIDITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_PRESSURE): sensor.sensor_schema(
                unit_of_measurement=UNIT_HECTOPASCAL,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_ATMOSPHERIC_PRESSURE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_WIND_SPEED): sensor.sensor_schema(
                unit_of_measurement=UNIT_METER_PER_SECOND,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_WIND_SPEED,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_WIND_DIRECTION_DEGREES): sensor.sensor_schema(
                unit_of_measurement=UNIT_DEGREES,
                accuracy_decimals=0,
                icon=ICON_SIGN_DIRECTION,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_WIND_GUST): sensor.sensor_schema(
                unit_of_measurement=UNIT_METER_PER_SECOND,
                accuracy_decimals=0,
                icon=ICON_WEATHER_WINDY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ACCUMULATED_PRECIPITATION): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETERS,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_PRECIPITATION,
                state_class=STATE_CLASS_TOTAL_INCREASING,
            ),
            cv.Optional(CONF_PRECIPITATION_INTENSITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETERS_PER_HOUR,
                accuracy_decimals=2,
                device_class=DEVICE_CLASS_PRECIPITATION_INTENSITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_LIGHT): sensor.sensor_schema(
                unit_of_measurement=UNIT_LUX,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_ILLUMINANCE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_UV_INTENSITY): sensor.sensor_schema(
                unit_of_measurement=UNIT_ULTRAVIOLET_INTENSITY,
                accuracy_decimals=1,
                icon=ICON_SUN_WIRELESS,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_UV_INDEX): sensor.sensor_schema(
                icon=ICON_SUN_WIRELESS,
                accuracy_decimals=0,
                device_class=STATE_CLASS_NONE,
            ),
        }
    ),
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_MISOL_ID])

    for key in TYPES:
        if sensor_config := config.get(key):
            sens = await sensor.new_sensor(sensor_config)
            cg.add(getattr(paren, f"set_{key}_sensor")(sens))
