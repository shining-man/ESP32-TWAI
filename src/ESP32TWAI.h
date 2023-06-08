// Copyright (c) 2023 Tobias Himmler
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INC_ESP32TWAI_H
#define INC_ESP32TWAI_H

#include <Arduino.h>
#include "driver/gpio.h"
#include "driver/twai.h"


#define ESP32TWAI_STAT_DRIVER_INSTALL   0x1
#define ESP32TWAI_STAT_START            0x2
#define ESP32TWAI_STAT_STOP             0x3
#define ESP32TWAI_STAT_UNINSTALL        0x4
#define ESP32TWAI_STAT_TRANSMIT         0x5
#define ESP32TWAI_STAT_RECEIVE          0x6
#define ESP32TWAI_STAT_SPEED            0x7
#define ESP32TWAI_STAT_STATUS           0x8


typedef enum
{
  TWAI_SPEED_100KBPS = 100,
  TWAI_SPEED_125KBPS = 125,
  TWAI_SPEED_250KBPS = 250,
  TWAI_SPEED_500KBPS = 500,
  TWAI_SPEED_800KBPS = 800,
  TWAI_SPEED_1MBPS   = 1000,
} TWAI_speed_s;

typedef enum
{
  TWAI_STD_FRAME = 0,
  TWAI_EXTD_FRAME = 1,
} TWAI_frame_type_s;


class ESP32TWAI
{
  public:
    esp_err_t begin(gpio_num_t rxPin, gpio_num_t txPin, TWAI_speed_s baud);
    void      setAlert(bool alert);
    esp_err_t stop();
    esp_err_t write(TWAI_frame_type_s extd, uint32_t identifier, uint8_t length, uint8_t *buffer);
    esp_err_t read(twai_message_t* ptr_message);
    uint32_t  getAlert();
    String    getErrorText(esp_err_t errNo);
  
  private:

};


extern ESP32TWAI CAN;

#endif


