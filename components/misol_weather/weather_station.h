#pragma once

#include <cstdint>
#include <vector>

#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"

#include "protocol.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

namespace esphome::misol_weather {

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
  void process_rx_buffer_(uint32_t now);
  void process_packet_(const protocol::WeatherPacket &packet);
  void reset_sub_entities_();

  bool first_data_received_{false};
  std::vector<uint8_t> rx_buffer_;
  uint32_t last_rx_byte_time_{0};
  uint32_t last_packet_time_{0};
};

}  // namespace esphome::misol_weather
