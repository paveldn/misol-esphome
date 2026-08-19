import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import (
    CONF_ID,
)

CODEOWNERS = ["@paveldn"]
DEPENDENCIES = ["uart"]

CONF_MISOL_ID = "misol_id"

misol_ns = cg.esphome_ns.namespace("misol_weather")
WeatherStation = misol_ns.class_("WeatherStation", uart.UARTDevice, cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(WeatherStation),
    }
).extend(uart.UART_DEVICE_SCHEMA)


FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "misol_weather",
    baud_rate=9600,
    require_tx=False,
    require_rx=True,
    parity="NONE",
    stop_bits=1,
    data_bits=8,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
