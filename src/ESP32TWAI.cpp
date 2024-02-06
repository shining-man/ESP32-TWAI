// Copyright (c) 2023 Tobias Himmler
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <cassert>
#include <array> // std::size
#include <string.h> // memcpy
#include "ESP32TWAI.h"

ESP32TWAI::ESP32TWAI() :
  _lastErrorFunction(DriverStatus::INIT)
{
}

ESP32TWAI::~ESP32TWAI()
{
  stop();
}

esp_err_t ESP32TWAI::begin(gpio_num_t rxPin,
                           gpio_num_t txPin,
                           TWAI_speed_s baud,
                           bool enableAlerts,
                           std::size_t rxQueueLength,
                           std::size_t txQueueLength)
{
  twai_general_config_t configGeneral = TWAI_GENERAL_CONFIG_DEFAULT(txPin, rxPin, TWAI_MODE_NORMAL);
  configGeneral.tx_queue_len = static_cast<uint32_t>(txQueueLength);
  configGeneral.rx_queue_len = static_cast<uint32_t>(rxQueueLength);

  configGeneral.alerts_enabled = (true == enableAlerts) ? TWAI_ALERT_ALL : TWAI_ALERT_NONE;

  const twai_filter_config_t configFilter = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  twai_timing_config_t configTiming {};

  switch(baud)
  {
    case TWAI_SPEED_100KBPS:
      configTiming = TWAI_TIMING_CONFIG_100KBITS();
      break;
    case TWAI_SPEED_125KBPS:
     configTiming = TWAI_TIMING_CONFIG_125KBITS();
      break;
    case TWAI_SPEED_250KBPS:
      configTiming = TWAI_TIMING_CONFIG_250KBITS();
      break;
    case TWAI_SPEED_500KBPS:
      configTiming = TWAI_TIMING_CONFIG_500KBITS();
      break;
    case TWAI_SPEED_800KBPS:
      configTiming = TWAI_TIMING_CONFIG_800KBITS();
      break;
    case TWAI_SPEED_1MBPS:
      configTiming = TWAI_TIMING_CONFIG_1MBITS();
      break;
    default:
      _lastErrorFunction = DriverStatus::SPEED;
      return ESP_ERR_NOT_SUPPORTED;
      break;
  }

  _lastErrorFunction = DriverStatus::DRIVER_INSTALL;
  const esp_err_t err = twai_driver_install(&configGeneral, &configTiming, &configFilter);
  if(err != ESP_OK)
    return err;

  _lastErrorFunction = DriverStatus::START;
  return twai_start();
}

esp_err_t ESP32TWAI::stop()
{
  _lastErrorFunction = DriverStatus::STOP;
  const esp_err_t err = twai_stop();
  if(err!=ESP_OK)
    return err;

  _lastErrorFunction = DriverStatus::UNINSTALL;
  return twai_driver_uninstall();
}

esp_err_t ESP32TWAI::write(TWAI_frame_type_s extd, uint32_t identifier, uint8_t length, uint8_t *buffer)
{
  twai_message_t tx_frame {};
  tx_frame.rtr = 0;
  tx_frame.flags = 0;
  tx_frame.extd = extd;                  // 0=11bit, 1=29bit
  tx_frame.data_length_code = length;
  tx_frame.identifier = identifier;

  _lastErrorFunction = DriverStatus::TRANSMIT;

  // check the buffer size
  if (std::size(tx_frame.data) < length)
    return ESP_ERR_NO_MEM;

  memcpy(tx_frame.data, buffer, length);

  return twai_transmit(&tx_frame, pdMS_TO_TICKS(100));
}

esp_err_t ESP32TWAI::read(twai_message_t* ptr_message)
{
  assert(ptr_message);

  twai_status_info_t statusInfo;

  _lastErrorFunction = DriverStatus::STATUS;
  const esp_err_t err = twai_get_status_info(&statusInfo);
  if(err == ESP_OK)
  {
    if(statusInfo.msgs_to_rx > 0)
    {
      _lastErrorFunction = DriverStatus::RECEIVE;
      return twai_receive(ptr_message, pdMS_TO_TICKS(10));
    }
    else
    {
      return ESP_OK;
    }
  }
  else
    return err;
}

uint32_t ESP32TWAI::getAlert() const
{
  uint32_t u32_lAlerts {};
  switch(twai_read_alerts(&u32_lAlerts, pdMS_TO_TICKS(20)))
  {
    case ESP_OK:
      return u32_lAlerts;
    case ESP_ERR_TIMEOUT:
      return 0x10000;
      break;
    case ESP_ERR_INVALID_ARG:
      return 0x20000;
      break;
    case ESP_ERR_INVALID_STATE:
      return 0x30000;
      break;
    default:
      return u32_lAlerts;
    break;
  }
}

twai_status_info_t ESP32TWAI::getStatus() const
{
  twai_status_info_t statusInfo {};
  (void) twai_get_status_info(&statusInfo); // return value not used
  return statusInfo;
}

std::string ESP32TWAI::getErrorText(esp_err_t errNo) const
{
  auto Stringify = [](DriverStatus status)
  {
    switch(status)
    {
      case DriverStatus::DRIVER_INSTALL:
        return std::string("TWAI INSTALL: ");
      case DriverStatus::START:
        return std::string("TWAI START: ");
      case DriverStatus::STOP:
        return std::string("TWAI STOP: ");
      case DriverStatus::UNINSTALL:
        return std::string("TWAI UNINSTALL: ");
      case DriverStatus::TRANSMIT:
        return std::string("TWAI TX: ");
      case DriverStatus::RECEIVE:
        return std::string("TWAI RX: ");
      case DriverStatus::SPEED:
        return std::string("TWAI SPEED: ");
      case DriverStatus::STATUS:
        return std::string("TWAI STATUS: ");
      default:
        return std::string(); // Return empty string
    }
  };

  return Stringify(_lastErrorFunction).append(esp_err_to_name(errNo));
}

/*static*/ ESP32TWAISingleton& ESP32TWAISingleton::instance()
{
  static ESP32TWAISingleton instance;

  return instance;
}