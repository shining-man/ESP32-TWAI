// Copyright (c) 2023 Tobias Himmler
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "ESP32TWAI.h"

ESP32TWAI ESP32Twai;

bool bo_mAlert=false;
uint8_t u8_mLastErrorFunction;
uint8_t u8_rxQueueLen=5;

esp_err_t ESP32TWAI::begin(gpio_num_t rxPin, gpio_num_t txPin, TWAI_speed_s baud)
{    
  twai_general_config_t configGeneral = TWAI_GENERAL_CONFIG_DEFAULT(txPin, rxPin, TWAI_MODE_NORMAL);
  configGeneral.tx_queue_len = 10;
  configGeneral.rx_queue_len = u8_rxQueueLen;
  if(bo_mAlert) configGeneral.alerts_enabled=TWAI_ALERT_ALL;
  else configGeneral.alerts_enabled=TWAI_ALERT_NONE;

  twai_filter_config_t configFilter = TWAI_FILTER_CONFIG_ACCEPT_ALL();
  twai_timing_config_t configTiming;

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
      u8_mLastErrorFunction=ESP32TWAI_STAT_SPEED;
      return ESP_ERR_NOT_SUPPORTED;
      break;
  }
  
  u8_mLastErrorFunction=ESP32TWAI_STAT_DRIVER_INSTALL;
  esp_err_t err = twai_driver_install(&configGeneral, &configTiming, &configFilter);
  if(err!=ESP_OK) return err;

  u8_mLastErrorFunction=ESP32TWAI_STAT_START;
  return twai_start();
}

void ESP32TWAI::setAlert(bool alert)
{
  bo_mAlert=alert;
}

void ESP32TWAI::setRxQueueLen(uint8_t rxQueueLen)
{
  u8_rxQueueLen = rxQueueLen;
}

esp_err_t ESP32TWAI::stop()
{
  u8_mLastErrorFunction=ESP32TWAI_STAT_STOP;
  esp_err_t err = twai_stop();
  if(err!=ESP_OK) return err;

  u8_mLastErrorFunction=ESP32TWAI_STAT_UNINSTALL;
  return twai_driver_uninstall();
}

esp_err_t ESP32TWAI::write(TWAI_frame_type_s extd, uint32_t identifier, uint8_t length, uint8_t *buffer)
{
  twai_message_t tx_frame;
  tx_frame.rtr = 0;
  tx_frame.flags = 0;
  tx_frame.extd = extd;                  // 0=11bit, 1=29bit
  tx_frame.data_length_code = length;
  tx_frame.identifier = identifier;   
  memcpy(tx_frame.data, buffer, length);

  u8_mLastErrorFunction=ESP32TWAI_STAT_TRANSMIT;
  return twai_transmit(&tx_frame, pdMS_TO_TICKS(100));
}

esp_err_t ESP32TWAI::read(twai_message_t* ptr_message)
{
  twai_status_info_t statusInfo;

  u8_mLastErrorFunction=ESP32TWAI_STAT_STATUS;
  esp_err_t err=twai_get_status_info(&statusInfo);
  if(err == ESP_OK)
  {
    if(statusInfo.msgs_to_rx>0)
    {
      u8_mLastErrorFunction=ESP32TWAI_STAT_RECEIVE;
      return twai_receive(ptr_message, pdMS_TO_TICKS(10));
    }
    else return ESP_OK;
  }
  else return err;
}


uint32_t ESP32TWAI::getAlert()
{
  uint32_t u32_lAlerts;
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


twai_status_info_t ESP32TWAI::getStatus()
{
  twai_status_info_t statusInfo;
  esp_err_t err=twai_get_status_info(&statusInfo);
  return statusInfo;
}

String ESP32TWAI::getErrorText(esp_err_t errNo)
{
  String errText="";
  switch(u8_mLastErrorFunction)
  {
    case ESP32TWAI_STAT_DRIVER_INSTALL:
      errText=F("TWAI INSTALL: ");
      break;
    case ESP32TWAI_STAT_START:
      errText=F("TWAI START: ");
      break;
    case ESP32TWAI_STAT_STOP:
      errText=F("TWAI STOP: ");
      break;
    case ESP32TWAI_STAT_UNINSTALL:
      errText=F("TWAI UNINSTALL: ");
      break;
    case ESP32TWAI_STAT_TRANSMIT:
      errText=F("TWAI TX: ");
      break;
    case ESP32TWAI_STAT_RECEIVE:
      errText=F("TWAI RX: ");
      break;
    case ESP32TWAI_STAT_SPEED:
      errText=F("TWAI SPPED: ");
      break;
    case ESP32TWAI_STAT_STATUS:
      errText=F("TWAI STATUS: ");
      break;
  }

  errText += esp_err_to_name(errNo);
  return errText;
}