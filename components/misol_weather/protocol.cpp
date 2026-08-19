#include "protocol.h"

#include <cmath>

namespace esphome {
namespace misol_weather {
namespace protocol {

namespace {

bool checksum_matches(const uint8_t *data, size_t start, size_t checksum_index) {
  uint8_t checksum = 0;
  for (size_t i = start; i < checksum_index; i++) {
    checksum += data[i];
  }
  return checksum == data[checksum_index];
}

float value_or_nan(uint32_t value, uint32_t invalid, float scale, float offset = 0.0f) {
  return value != invalid ? (static_cast<float>(value) + offset) * scale : NAN;
}

}  // namespace

PacketType detect_packet_type(const uint8_t *data, size_t len) {
  if (len < BASIC_PACKET_SIZE || data[0] != PACKET_HEADER || !checksum_matches(data, 0, 16)) {
    return PacketType::INVALID;
  }

  if (len >= PRESSURE_PACKET_SIZE && checksum_matches(data, 17, 20)) {
    return PacketType::BASIC_WITH_PRESSURE;
  }

  return PacketType::BASIC;
}

bool parse_packet(const uint8_t *data, size_t len, WeatherPacket *packet) {
  PacketType type = detect_packet_type(data, len);
  if (type == PacketType::INVALID || packet == nullptr) {
    return false;
  }

  WeatherPacket decoded;
  decoded.type = type;
  decoded.low_battery = (data[3] & 0x08) != 0;

  const uint16_t wind_direction = data[2] + (static_cast<uint16_t>(data[3] & 0x80) << 1);
  decoded.wind_direction_degrees = value_or_nan(wind_direction, 0x1FF, 1.0f);

  const uint16_t temperature = data[4] + (static_cast<uint16_t>(data[3] & 0x07) << 8);
  decoded.temperature = value_or_nan(temperature, 0x7FF, 0.1f, -400.0f);

  decoded.humidity = data[5];

  const uint16_t wind_speed = data[6] + (static_cast<uint16_t>(data[3] & 0x10) << 4);
  decoded.wind_speed = value_or_nan(wind_speed, 0x1FF, 1.12f / 8.0f);

  decoded.wind_gust = value_or_nan(data[7], 0xFF, 1.12f);
  decoded.accumulated_precipitation = data[9] + (static_cast<uint16_t>(data[8]) << 8);

  const uint16_t uv_intensity = data[11] + (static_cast<uint16_t>(data[10]) << 8);
  decoded.uv_intensity = value_or_nan(uv_intensity, 0xFFFF, 0.1f);
  decoded.uv_index = uv_intensity != 0xFFFF ? uv_intensity / 400.0f : NAN;

  const uint32_t light = data[14] + (static_cast<uint32_t>(data[13]) << 8) + (static_cast<uint32_t>(data[12]) << 16);
  decoded.light = value_or_nan(light, 0xFFFFFF, 0.1f);

  if (type == PacketType::BASIC_WITH_PRESSURE) {
    const uint32_t pressure =
        (static_cast<uint32_t>(data[17]) << 16) + (static_cast<uint32_t>(data[18]) << 8) + data[19];
    decoded.pressure = pressure / 100.0f;
  }

  *packet = decoded;
  return true;
}

}  // namespace protocol
}  // namespace misol_weather
}  // namespace esphome
