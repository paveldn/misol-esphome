#pragma once

#include <chrono>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
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
  SUB_SENSOR(accumulated_precipitation)
  SUB_SENSOR(uv_intensity)
  SUB_SENSOR(uv_index)
  SUB_SENSOR(light)
#endif
#ifdef USE_BINARY_SENSOR
  SUB_BINARY_SENSOR(battery_level)
#endif
#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(wind_direction)
  SUB_TEXT_SENSOR(wind_speed)
#endif
 public:
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void loop() override;
 protected:
  PacketType check_packet_(const uint8_t *data, size_t len);
  void process_packet_(const uint8_t *data, size_t len, bool has_pressure);
  void reset_sub_entities_();
  bool first_data_received_{false};
  std::chrono::steady_clock::time_point last_packet_time_;
};

} // misol
} // esphome