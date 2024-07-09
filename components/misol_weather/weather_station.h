#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace misol {

  enum class PacketType {
    WRONG_PACKET = -1,
    BASIC_PACKET = 0,
    BASIC_WITH_PRESSURE,
  };

class WeatherStation : public PollingComponent, public uart::UARTDevice {
 public:
  float get_setup_priority() const override { return setup_priority::HARDWARE; }
  void loop() override;
  void update() override;
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