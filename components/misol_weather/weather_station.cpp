#include "weather_station.h"

#include "esphome/core/helpers.h"

namespace esphome::misol_weather {

static const char *const TAG = "misol_weather";
static constexpr uint32_t COMMUNICATION_TIMEOUT_MS = 2 * 60 * 1000;
static constexpr uint32_t PACKET_GAP_TIMEOUT_MS = 50;
static constexpr size_t MAX_RX_BUFFER_SIZE = protocol::PRESSURE_PACKET_SIZE * 3;

void WeatherStation::dump_config() {
  ESP_LOGCONFIG(TAG, "Misol Weather Station:");
  this->check_uart_settings(9600);
#ifdef USE_SENSOR
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("  ", "Humidity", this->humidity_sensor_);
  LOG_SENSOR("  ", "Pressure", this->pressure_sensor_);
  LOG_SENSOR("  ", "Wind Speed", this->wind_speed_sensor_);
  LOG_SENSOR("  ", "Wind Gust", this->wind_gust_sensor_);
  LOG_SENSOR("  ", "Wind Direction Degrees", this->wind_direction_degrees_sensor_);
  LOG_SENSOR("  ", "Accumulated Precipitation", this->accumulated_precipitation_sensor_);
  LOG_SENSOR("  ", "UV Intensity", this->uv_intensity_sensor_);
  LOG_SENSOR("  ", "UV Index", this->uv_index_sensor_);
  LOG_SENSOR("  ", "Light", this->light_sensor_);
#endif
#ifdef USE_BINARY_SENSOR
  LOG_BINARY_SENSOR("  ", "Battery Level", this->battery_level_binary_sensor_);
#endif
}

void WeatherStation::loop() {
  const uint32_t now = millis();
  if (this->first_data_received_ && now - this->last_packet_time_ > COMMUNICATION_TIMEOUT_MS) {
    ESP_LOGW(TAG, "Communication timeout");
    this->first_data_received_ = false;
    this->reset_sub_entities_();
  }

  while (this->available() > 0) {
    this->rx_buffer_.push_back(this->read());
    this->last_rx_byte_time_ = now;
  }
  if (!this->rx_buffer_.empty()) {
    this->process_rx_buffer_(now);
  }
}

void WeatherStation::reset_sub_entities_() {
#ifdef USE_SENSOR
  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(NAN);
  if (this->humidity_sensor_ != nullptr)
    this->humidity_sensor_->publish_state(NAN);
  if (this->pressure_sensor_ != nullptr)
    this->pressure_sensor_->publish_state(NAN);
  if (this->wind_speed_sensor_ != nullptr)
    this->wind_speed_sensor_->publish_state(NAN);
  if (this->wind_gust_sensor_ != nullptr)
    this->wind_gust_sensor_->publish_state(NAN);
  if (this->wind_direction_degrees_sensor_ != nullptr)
    this->wind_direction_degrees_sensor_->publish_state(NAN);
  if (this->accumulated_precipitation_sensor_ != nullptr)
    this->accumulated_precipitation_sensor_->publish_state(NAN);
  if (this->uv_intensity_sensor_ != nullptr)
    this->uv_intensity_sensor_->publish_state(NAN);
  if (this->light_sensor_ != nullptr)
    this->light_sensor_->publish_state(NAN);
  if (this->uv_index_sensor_ != nullptr)
    this->uv_index_sensor_->publish_state(NAN);
#endif  // USE_SENSOR
}

void WeatherStation::process_rx_buffer_(uint32_t now) {
  size_t attempts_remaining = this->rx_buffer_.size();
  while (!this->rx_buffer_.empty()) {
    if (attempts_remaining == 0) {
      ESP_LOGW(TAG, "Packet processing limit reached with %u bytes buffered",
               static_cast<unsigned>(this->rx_buffer_.size()));
      break;
    }
    attempts_remaining--;

    size_t header = protocol::find_packet_header(this->rx_buffer_.data(), this->rx_buffer_.size());
    if (header == protocol::HEADER_NOT_FOUND) {
      ESP_LOGW(TAG, "Dropping %u bytes without packet header", static_cast<unsigned>(this->rx_buffer_.size()));
      this->rx_buffer_.clear();
      break;
    }
    if (header > 0) {
      ESP_LOGW(TAG, "Dropping %u bytes before packet header", static_cast<unsigned>(header));
      this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + header);
    }

    if (this->rx_buffer_.size() < protocol::BASIC_PACKET_SIZE) {
      break;
    }

    protocol::PacketType type = protocol::detect_packet_type(this->rx_buffer_.data(), this->rx_buffer_.size());
    if (type == protocol::PacketType::INVALID) {
      ESP_LOGW(TAG, "Dropping invalid packet candidate: %s",
               format_hex_pretty(this->rx_buffer_.data(), protocol::BASIC_PACKET_SIZE).c_str());
      size_t next_header = protocol::find_packet_header(this->rx_buffer_.data(), this->rx_buffer_.size(), 1);
      if (next_header == protocol::HEADER_NOT_FOUND) {
        this->rx_buffer_.clear();
        break;
      }
      this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + next_header);
      continue;
    }

    if (type == protocol::PacketType::BASIC && this->rx_buffer_.size() < protocol::PRESSURE_PACKET_SIZE &&
        now - this->last_rx_byte_time_ < PACKET_GAP_TIMEOUT_MS) {
      break;
    }

    const size_t packet_size =
        type == protocol::PacketType::BASIC_WITH_PRESSURE ? protocol::PRESSURE_PACKET_SIZE : protocol::BASIC_PACKET_SIZE;
    protocol::WeatherPacket packet;
    if (protocol::parse_packet(this->rx_buffer_.data(), packet_size, &packet)) {
      ESP_LOGD(TAG, "%s received: %s", this->first_data_received_ ? "Packet" : "First packet",
               format_hex_pretty(this->rx_buffer_.data(), packet_size).c_str());
      this->first_data_received_ = true;
      this->last_packet_time_ = now;
      this->process_packet_(packet);
    }
    this->rx_buffer_.erase(this->rx_buffer_.begin(), this->rx_buffer_.begin() + packet_size);
  }

  if (this->rx_buffer_.size() > MAX_RX_BUFFER_SIZE) {
    ESP_LOGW(TAG, "UART buffer overflow, clearing %u bytes", static_cast<unsigned>(this->rx_buffer_.size()));
    this->rx_buffer_.clear();
  }
}

void WeatherStation::process_packet_(const protocol::WeatherPacket &packet) {
#ifdef USE_SENSOR
  if (this->pressure_sensor_ != nullptr) {
    this->pressure_sensor_->publish_state(packet.pressure);
  }
  if (this->wind_direction_degrees_sensor_ != nullptr) {
    this->wind_direction_degrees_sensor_->publish_state(packet.wind_direction_degrees);
  }
  if (this->temperature_sensor_ != nullptr) {
    this->temperature_sensor_->publish_state(packet.temperature);
  }
  if (this->humidity_sensor_ != nullptr) {
    this->humidity_sensor_->publish_state(packet.humidity);
  }
  if (this->wind_speed_sensor_ != nullptr) {
    this->wind_speed_sensor_->publish_state(packet.wind_speed);
  }
  if (this->wind_gust_sensor_ != nullptr) {
    this->wind_gust_sensor_->publish_state(packet.wind_gust);
  }

  if (this->accumulated_precipitation_sensor_ != nullptr) {
    this->accumulated_precipitation_sensor_->publish_state(packet.accumulated_precipitation * 0.3f);
  }
  if (this->uv_intensity_sensor_ != nullptr) {
    this->uv_intensity_sensor_->publish_state(packet.uv_intensity);
  }
  if (this->uv_index_sensor_ != nullptr) {
    this->uv_index_sensor_->publish_state(packet.uv_index);
  }
  if (this->light_sensor_ != nullptr) {
    this->light_sensor_->publish_state(packet.light);
  }
#endif  // USE_SENSOR

#ifdef USE_BINARY_SENSOR
  if (this->battery_level_binary_sensor_ != nullptr) {
    this->battery_level_binary_sensor_->publish_state(packet.low_battery);
  }
#endif  // USE_BINARY_SENSOR
}

}  // namespace esphome::misol_weather
