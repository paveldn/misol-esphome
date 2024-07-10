import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_TEMPERATURE,
    CONF_HUMIDITY,
    CONF_PRESSURE,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_HECTOPASCAL,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_ATMOSPHERIC_PRESSURE,
    STATE_CLASS_MEASUREMENT,
    CONF_WIND_DIRECTION_DEGREES,
    UNIT_DEGREES,
    ICON_SIGN_DIRECTION,
    CONF_WIND_SPEED,
    DEVICE_CLASS_WIND_SPEED,
    ICON_WEATHER_WINDY,
    UNIT_MILLIMETER,
    CONF_LIGHT,
    UNIT_LUX,
    DEVICE_CLASS_ILLUMINANCE,
)
from . import (
    CONF_MISOL_ID,
    WeatherStation,
)

CODEOWNERS = ["@paveldn"]

UNIT_METER_PER_SECOND = "m/s"
CONF_WIND_GUST = "wind_gust"
CONF_RAINFALL = "rainfall"
ICON_WEATHER_POURING = "mdi:weather-pouring"

TYPES = [ 
    CONF_TEMPERATURE, 
    CONF_HUMIDITY, 
    CONF_PRESSURE, 
    CONF_WIND_SPEED, 
    CONF_WIND_DIRECTION_DEGREES 
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
            cv.Optional(CONF_RAINFALL): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETER,
                accuracy_decimals=1,
                icon=ICON_WEATHER_POURING,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_LIGHT): sensor.sensor_schema(
                unit_of_measurement=UNIT_LUX,
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_ILLUMINANCE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
)

async def to_code(config):
    paren = await cg.get_variable(config[CONF_MISOL_ID])

    for key in TYPES:
        if sensor_config := config.get(key):
            sens = await sensor.new_sensor(sensor_config)
            cg.add(getattr(paren, f"set_{key}_sensor")(sens))
