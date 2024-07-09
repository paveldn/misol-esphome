#include "esphome/components/uart/uart.h"
#include "esphome/core/helpers.h"
#include "weather_station.h"
#include <memory>

namespace esphome {
namespace misol {

static const char *TAG = "misol.weather_station"; 

std::string angleToDirection(double angle) {
    const char* directions[] = {
        "N", "NNE", "NE", "ENE", "E", "ESE", "SE", "SSE",
        "S", "SSW", "SW", "WSW", "W", "WNW", "NW", "NNW", "N"
    };
    int index = static_cast<int>((angle + 11.25) / 22.5);
    return directions[index % 16];
}

void WeatherStation::update() {
}

void WeatherStation::loop() {
  auto size = this->available();
  if (size > 0) {
    ESP_LOGD(TAG, "Available: %d", size);
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[size]);
    for (int i = 0; i < size; i++) {
      buffer[i] = this->read();
    }
    ESP_LOGD(TAG, "Received: %s", format_hex_pretty(buffer.get(), size).c_str());
    PacketType packet_type = check_packet(buffer.get(), size);
    switch (packet_type) {
      case PacketType::WRONG_PACKET:
        ESP_LOGW(TAG, "Unknown packet received: %s", format_hex_pretty(buffer.get(), size).c_str());
        break;
      case PacketType::BASIC_WITH_PRESSURE:
        {
          float pressure = ((((uint32_t) buffer[17]) << 16) + (((uint32_t) buffer[18]) << 8) + buffer[19]) / 100.0;
          if (!this->pressure_.has_value() || (this->pressure_.value() != pressure)) {
            this->new_data_ = true;
            this->pressure_ = pressure;
          }
          ESP_LOGI(TAG, "  Pressure: %0.2f", pressure);
        }
      case PacketType::BASIC_PACKET:
        {
          unsigned int wind_direction = buffer[2] + (((uint16_t)(buffer[3] && 0x80)) << 1);
          if (!this->wind_direction_.has_value() || (this->wind_direction_.value() != wind_direction)) {
            this->new_data_ = true;
            this->wind_direction_ = wind_direction;
          }
          ESP_LOGI(TAG, "  Wind direction: %d° (%s)", wind_direction, angleToDirection(wind_direction).c_str());
          bool low_battery = buffer[3] & 0x08 != 0;
          if (!this->low_battery_.has_value() || (this->low_battery_.value() != low_battery)) {
            this->new_data_ = true;
            this->low_battery_ = low_battery;
          }
          ESP_LOGI(TAG, "  Low battery: %s", low_battery ? "Yes" : "No");
          float temperature = (buffer[4] + (((uint16_t)(buffer[3] & 0x07)) << 8) - 400) / 10.0;
          if (!this->temperature_.has_value() || (this->temperature_.value() != temperature)) {
            this->new_data_ = true;
            this->temperature_ = temperature;
          }
          ESP_LOGI(TAG, "  Temperature: %0.1f°C", temperature);
          unsigned int humidity = buffer[5];
          if (!this->humidity_.has_value() || (this->humidity_.value() != humidity)) {
            this->new_data_ = true;
            this->humidity_ = humidity;
          }
          ESP_LOGI(TAG, "  Humidity: %d%%", humidity);
          float wind_speed = ((float)(buffer[6] + (((uint16_t)(buffer[3] & 0x10)) << 4))) / 8.0 * 1.12;
          if (!this->wind_speed_.has_value() || (this->wind_speed_.value() != wind_speed)) {
            this->new_data_ = true;
            this->wind_speed_ = wind_speed;
          }
          ESP_LOGI(TAG, "  Wind speed: %0.2f m/s", wind_speed);
          float wind_gust = buffer[7] * 1.12;
          if (!this->wind_gust_.has_value() || (this->wind_gust_.value() != wind_gust)) {
            this->new_data_ = true;
            this->wind_gust_ = wind_gust;
          }
          ESP_LOGI(TAG, "  Wind gust: %0.2f m/s", wind_gust);
          float rain_fall = (buffer[9] + (((uint16_t)buffer[8]) << 8)) * 0.3;
          if (!this->rain_fall_.has_value() || (this->rain_fall_.value() != rain_fall)) {
            this->new_data_ = true;
            this->rain_fall_ = rain_fall;
          }
          ESP_LOGI(TAG, "  Rain fall: %0.1f mm", rain_fall);
          unsigned int uv_value = buffer[11] + (((uint16_t)buffer[10]) << 8);
          if (!this->uv_value_.has_value() || (this->uv_value_.value() != uv_value)) {
            this->new_data_ = true;
            this->uv_value_ = uv_value;
          }
          ESP_LOGI(TAG, "  UV value: %d uW/cm²", uv_value);
          float light = (buffer[14] + (buffer[13] << 8) + (buffer[12] << 16)) / 10.0;
          if (!this->light_.has_value() || (this->light_.value() != light)) {
            this->new_data_ = true;
            this->light_ = light;
          }
          ESP_LOGI(TAG, "  Light: %0.1f Lux", light);
        }
        break;
    }
  }
}

PacketType WeatherStation::check_packet(const uint8_t *data, size_t len) {
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



} // misol
} // esphome


