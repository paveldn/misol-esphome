Weather Station Component
=========================

Work in progress...

This is an implementation of the ESPHome component for a Misol weather station with RS485 output. The weather station is a combination of various sensors, including temperature, humidity, pressure (not available for all models), wind speed, wind direction, wind gust, accumulated precipitation, light, UV intensity, and UV index.

This component requires a `UART Bus <https://esphome.io/components/uart#uart>`_ to be setup.

Component/Hub
-------------

The component acts as a hub for the weather station sensors. It reads the data from the weather station and provides it to the sensors.

Example configuration:
----------------------

.. code-block:: yaml

    misol_weather:
      id: weather_station
      uart_id: uart_bus

Configuration variables:
------------------------

- **uart_id** (Required, ID): The ID of the UART bus to use for communication with the weather station.

Sensor
------

The sensor platform allows you to get the data from the weather station.

Example configuration:
----------------------

.. code-block:: yaml

    sensor:
      - platform: misol_weather
        misol_id: weather_station
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
          name: Weather station Accumulated 
        precipitation_intensity:
          name: Weather station Precipitation Intensity
        light:
          name: Weather station Light
        uv_intensity:
          name: Weather station Ultraviolet Intensity
        uv_index:
          name: Weather station Ultraviolet Index


Configuration variables:
------------------------

- **misol_id** (Required, ID): The ID of the weather station component.
- **temperature** (Optional): The temperature sensor.
- **humidity** (Optional): The humidity sensor.
- **pressure** (Optional): The pressure sensor.
- **wind_speed** (Optional): The wind speed sensor.
- **wind_direction_degrees** (Optional): The wind direction sensor in degrees.
- **wind_gust** (Optional): The wind gust sensor.
- **accumulated_precipitation** (Optional): The accumulated precipitation sensor.
- **precipitation_intensity** (Optional): The precipitation intensity sensor.
- **light** (Optional): The light sensor.
- **uv_intensity** (Optional): The UV intensity sensor.
- **uv_index** (Optional): The UV index sensor.

Binary Sensor
-------------

The binary sensor platform allows you to get the battery level of the weather station.

Example configuration:
----------------------

.. code-block:: yaml

    binary_sensor:
      - platform: misol_weather
        misol_id: weather_station
        battery_level:
          name: Weather station Battery Level

Configuration variables:
------------------------

- **misol_id** (Required, ID): The ID of the weather station component.
- **battery_level** (Required): The battery level sensor.

Text Sensor
-----------

The text sensor platform allows you to get the wind direction in text format.

Example configuration:
----------------------

.. code-block:: yaml

    text_sensor:
      - platform: misol_weather
        misol_id: weather_station
        light:
          name: Weather station Light Text
        wind_direction:
          name: Weather station Wind Direction Text
        wind_speed:
          name: Weather station Wind Speed Text

Configuration variables:
------------------------

- **misol_id** (Required, ID): The ID of the weather station component.
- **light** (Optional): The light sensor in text format.
- **wind_direction** (Optional): The wind direction sensor in text format.
- **wind_speed** (Optional): The wind speed sensor in text format.

See Also
--------

- `ESPHome Sensor <https://esphome.io/components/sensor/index.html>`_
- `ESPHome Binary Sensor <https://esphome.io/components/binary_sensor/index.html>`_
- `ESPHome Text Sensor <https://esphome.io/components/text_sensor/index.html>`_
