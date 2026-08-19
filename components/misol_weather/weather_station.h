#pragma once

#include <chrono>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "protocol.h"
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

namespace esphome {
namespace misol_weather {

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

 public:
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void dump_config() override;
  void loop() override;

 protected:
  void process_rx_buffer_(const std::chrono::steady_clock::time_point &now);
  void process_packet_(const protocol::WeatherPacket &packet, const std::chrono::steady_clock::time_point &now);
  void reset_sub_entities_();
  bool first_data_received_{false};
  std::vector<uint8_t> rx_buffer_;
  std::chrono::steady_clock::time_point last_rx_byte_time_;
  std::chrono::steady_clock::time_point last_packet_time_;
};

}  // namespace misol_weather
}  // namespace esphome
