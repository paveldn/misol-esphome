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
  SUB_SENSOR(precipitation_intensity)
#endif
#ifdef USE_BINARY_SENSOR
  SUB_BINARY_SENSOR(battery_level)
  SUB_BINARY_SENSOR(night)
  void set_upper_night_threshold(float upper_night_threshold) { this->upper_night_threshold_ = upper_night_threshold; };
  void set_lower_night_threshold(float lower_night_threshold) { this->lower_night_threshold_ = lower_night_threshold; };
#endif
  void set_precipitation_intensity_interval(unsigned int precipitation_intensity_interval) {
    this->precipitation_intensity_interval_ = std::chrono::minutes(precipitation_intensity_interval);
  }

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
  std::chrono::steady_clock::time_point last_packet_time_;
  std::chrono::milliseconds precipitation_intensity_interval_{std::chrono::minutes(5)};
  std::chrono::steady_clock::time_point previous_precipitation_timestamp_;
  esphome::optional<uint16_t> previous_precipitation_{};
#ifdef USE_BINARY_SENSOR
  bool detect_night_(float uv_intensity);
  bool night_state_{false};
  bool night_state_initialized_{false};
  float upper_night_threshold_{5.5};
  float lower_night_threshold_{4.5};
#endif  // USE_BINARY_SENSOR
};

}  // namespace misol_weather
}  // namespace esphome
