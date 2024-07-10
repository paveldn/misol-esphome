#include "esphome/components/uart/uart.h"
#include "esphome/core/helpers.h"
#include "weather_station.h"
#include <memory>

namespace {

std::string angle_to_direction(double angle) {
    const char* directions[] = {
        "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
        "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW", "N"
    };
    int index = static_cast<int>((angle + 11.25) / 22.5);
    return directions[index % 16];
}

} // namespace

namespace esphome {
namespace misol {

static const char *TAG = "misol.weather_station"; 

void WeatherStation::loop() {
  auto size = this->available();
  if (size > 0) {
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[size]);
    for (int i = 0; i < size; i++) {
      buffer[i] = this->read();
    }
    ESP_LOGD(TAG, "Received: %s", format_hex_pretty(buffer.get(), size).c_str());
    PacketType packet_type = check_packet_(buffer.get(), size);
    if (packet_type != PacketType::WRONG_PACKET) {
      this->process_packet_(buffer.get(), size, packet_type == PacketType::BASIC_WITH_PRESSURE);
    } else {
      ESP_LOGW(TAG, "Unknown packet received: %s", format_hex_pretty(buffer.get(), size).c_str());
    }

  }
}

PacketType WeatherStation::check_packet_(const uint8_t *data, size_t len) {
  // Checking basic packet
  if (len < 17) {
    return PacketType::WRONG_PACKET;
  }
  if (data[0] != 0x24) {
    return PacketType::WRONG_PACKET;
  }
  uint8_t checksum = 0;
  for (int i = 0; i < 16; i++) {
    checksum += data[i];
  }
  if (checksum != data[16]) {
    return PacketType::WRONG_PACKET;
  }
  // Checking barometry pressure packet
  if (len > 17) {
    if (len < 21) {
      return PacketType::BASIC_PACKET;
    }
    checksum = 0;
    for (int i = 17; i < 20; i++) {
      checksum += data[i];
    }
    if (checksum != data[20]) {
      return PacketType::BASIC_PACKET;
    }
    return PacketType::BASIC_WITH_PRESSURE;
  }
  return PacketType::BASIC_PACKET;
}

void WeatherStation::process_packet_(const uint8_t *data, size_t len, bool has_pressure) {
#ifdef USE_SENSOR
  if (this->pressure_sensor_ != nullptr) {
    if (has_pressure) {
      uint32_t pressure = (((uint32_t)data[17]) << 16) + (((uint32_t)data[18]) << 8) + data[19];
      this->pressure_sensor_->publish_state(pressure / 100.0f);
    } else {
      this->pressure_sensor_->publish_state(NAN);
    }
  }
#endif // USE_SENSOR
#ifdef USE_SENSOR
  if (this->wind_direction_degrees_sensor_ != nullptr) {
    uint16_t wind_direction = data[2] + (((uint16_t)(data[3] & 0x80)) << 1);
    if (wind_direction != 0x1FF) {
      this->wind_direction_degrees_sensor_->publish_state(wind_direction);
    } else {
      this->wind_direction_degrees_sensor_->publish_state(NAN);
    }
  }
#endif // USE_SENSOR
//#ifdef USE_BINARY_SENSOR
//  bool low_battery = data[3] & 0x08 != 0;
//  if (!this->low_battery_.has_value() || (this->low_battery_.value() != low_battery)) {
//    this->new_data_ = true;
//    this->low_battery_ = low_battery;
//  }
//  ESP_LOGI(TAG, "  Low battery: %s", low_battery ? "Yes" : "No");
//#endif // USE_BINARY_SENSOR
#ifdef USE_SENSOR
  if (this->temperature_sensor_ != nullptr) {
    uint16_t temperature = data[4] + (((uint16_t)(data[3] & 0x07)) << 8);
    if (temperature != 0x7FF) {
      this->temperature_sensor_->publish_state((temperature - 400) / 10.0);
    } else {
      this->temperature_sensor_->publish_state(NAN);
    }
  }
#endif // USE_SENSOR
#ifdef USE_SENSOR
  if (this->humidity_sensor_ != nullptr) {
    uint8_t humidity = data[5];
    if (humidity != 0xFF) {
      this->humidity_sensor_->publish_state(humidity);
    } else {
      this->humidity_sensor_->publish_state(NAN);
    }
  }
#endif // USE_SENSOR
#ifdef USE_SENSOR
  if (this->wind_speed_sensor_ != nullptr) {
    uint16_t wind_speed = data[6] + (((uint16_t)(data[3] & 0x10)) << 4);
    if (wind_speed != 0x1FF) {
      this->wind_speed_sensor_->publish_state(wind_speed / 8.0 * 1.12);
    } else {
      this->wind_speed_sensor_->publish_state(NAN);
    }
  }
#endif // USE_SENSOR
#ifdef USE_SENSOR
  if (this->wind_gust_sensor_ != nullptr) {
    uint8_t wind_gust = data[7];
    if (wind_gust != 0xFF) {
      this->wind_gust_sensor_->publish_state(wind_gust * 1.12);
    } else {
      this->wind_gust_sensor_->publish_state(NAN);
    }
  }
#endif // USE_SENSOR
#ifdef USE_SENSOR
  if (this->rainfall_sensor_ != nullptr) {
    uint16_t rainfall = data[9] + (((uint16_t)data[8]) << 8);
    this->rainfall_sensor_->publish_state(rainfall * 0.3);
  }
#endif // USE_SENSOR
#ifdef USE_SENSOR
  if (this->uv_value_sensor_ != nullptr) {
    unsigned int uv_value = data[11] + (((uint16_t)data[10]) << 8);
    if (uv_value != 0xFFFF) {
      this->uv_value_sensor_->publish_state(uv_value);
    } else {
      this->uv_value_sensor_->publish_state(NAN);
    }
  }
#endif // USE_SENSOR
#ifdef USE_SENSOR
  if (this->light_sensor_ != nullptr) {
    uint32_t light = (data[14] + (data[13] << 8) + (data[12] << 16));
    if (light != 0xFFFFFF) {
      this->light_sensor_->publish_state(light / 10.0);
    } else {
      this->light_sensor_->publish_state(NAN);
    }
  }
#endif // USE_SENSOR
}

} // misol
} // esphome


