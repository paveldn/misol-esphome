#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

namespace esphome {
namespace misol {

  enum class PacketType {
    WRONG_PACKET = -1,
    BASIC_PACKET = 0,
    BASIC_WITH_PRESSURE,
  };

class WeatherStation : public Component, public uart::UARTDevice {
#ifdef USE_SENSOR
  SUB_SENSOR(temperature)
  SUB_SENSOR(humidity)
  SUB_SENSOR(pressure)
  SUB_SENSOR(wind_speed)
  SUB_SENSOR(wind_gust)
  SUB_SENSOR(wind_direction_degrees)
  SUB_SENSOR(rain_fall)
  SUB_SENSOR(uv_value)
  SUB_SENSOR(light)
#endif
#ifdef USE_BINARY_SENSOR
  SUB_BINARY_SENSOR(low_battery)
#endif
 public:
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void loop() override;
  PacketType check_packet(const uint8_t *data, size_t len);
 protected:
  bool new_data_{false};
  optional<unsigned int> wind_direction_{};
  optional<bool> low_battery_{};
  optional<float> temperature_{};
  optional<unsigned int> humidity_{};
  optional<float> wind_speed_{};
  optional<float> wind_gust_{};
  optional<float> rain_fall_{};
  optional<int> uv_value_{};
  optional<float> light_{};
  optional<float> pressure_{};
};

} // misol
} // esphome