Weather Station Component
=========================

Work in progress...

This is an implementation of the ESPHome component for a Misol weather station with RS485 output. The weather station is a combination of various sensors, including temperature, humidity, pressure (not available for all models), wind speed, wind direction, wind gust, accumulated precipitation, light, UV intensity, and UV index.

This component requires a `UART Bus <https://esphome.io/components/uart#uart>`_ to be setup.

Example configuration:
----------------------

.. code-block:: yaml

    uart:
      rx_pin: GPIO3
      baud_rate: 9600
      stop_bits: 1

    misol_weather:

    sensor:
      - platform: misol_weather
        temperature:
          name: Weather station Temperature
        humidity:
          name: Weather station Humidity
        pressure:
          name: Weather station Pressure
        wind_speed:
          name: Weather station Wind Speed
        wind_direction_degrees:
          name: Weather station Wind Direction
        wind_gust:
          name: Weather station Wind Gust
        accumulated_precipitation:
          name: Weather station Accumulated Precipitation 
        light:
          name: Weather station Light
        uv_intensity:
          name: Weather station Ultraviolet Intensity
        uv_index:
          name: Weather station Ultraviolet Index

    binary_sensor:
      - platform: misol_weather
        battery_level:
          name: Weather station Battery Level

    text_sensor:
      - platform: misol_weather
        wind_direction:
          name: Weather station Wind Direction Text
        wind_speed:
          name: Weather station Wind Speed Text



Configuration variables:
------------------------

