#pragma once

#include <cstddef>
#include <cstdint>
#include <cmath>

namespace esphome {
namespace misol_weather {
namespace protocol {

static constexpr uint8_t PACKET_HEADER = 0x24;
static constexpr size_t BASIC_PACKET_SIZE = 17;
static constexpr size_t PRESSURE_PACKET_SIZE = 21;

enum class PacketType {
  INVALID = 0,
  BASIC,
  BASIC_WITH_PRESSURE,
};

struct WeatherPacket {
  PacketType type{PacketType::INVALID};
  float temperature{NAN};
  float humidity{NAN};
  float pressure{NAN};
  float wind_speed{NAN};
  float wind_gust{NAN};
  float wind_direction_degrees{NAN};
  uint16_t accumulated_precipitation{0};
  float uv_intensity{NAN};
  float uv_index{NAN};
  float light{NAN};
  bool low_battery{false};
};

PacketType detect_packet_type(const uint8_t *data, size_t len);
bool parse_packet(const uint8_t *data, size_t len, WeatherPacket *packet);

}  // namespace protocol
}  // namespace misol_weather
}  // namespace esphome
