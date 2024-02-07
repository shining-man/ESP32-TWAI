// Copyright (c) 2023 Tobias Himmler
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INC_ESP32TWAI_H
#define INC_ESP32TWAI_H

#include <cstddef>
#include <string>
#include <driver/gpio.h>
#include <driver/twai.h>

// TODO: Would be nice to put this module into an own namespce

enum TWAI_speed_s
{
  TWAI_SPEED_100KBPS = 100,
  TWAI_SPEED_125KBPS = 125,
  TWAI_SPEED_250KBPS = 250,
  TWAI_SPEED_500KBPS = 500,
  TWAI_SPEED_800KBPS = 800,
  TWAI_SPEED_1MBPS   = 1000
};

enum TWAI_frame_type_s
{
  TWAI_STD_FRAME  = 0,
  TWAI_EXTD_FRAME = 1
};

class ESP32TWAI
{

  public:
    ESP32TWAI();
    ~ESP32TWAI();

  public:
    esp_err_t begin(gpio_num_t rxPin,
                    gpio_num_t txPin,
                    TWAI_speed_s baud,
                    bool enableAlerts = false,
                    std::size_t rxQueueLength = RX_QUEUE_LENGTH_DEFAULT,
                    std::size_t txQueueLength = TX_QUEUE_LENGTH_DEFAULT);
    esp_err_t   stop();
    esp_err_t   write(TWAI_frame_type_s extd, uint32_t identifier, uint8_t length, uint8_t *buffer);
    esp_err_t   read(twai_message_t* ptr_message);
    uint32_t    getAlert() const;
    twai_status_info_t getStatus() const;
    std::string getErrorText(esp_err_t errNo) const;

  private:
    enum class DriverStatus : uint8_t
    {
      INIT           = 0x0,
      DRIVER_INSTALL = 0x1,
      START          = 0x2,
      STOP           = 0x3,
      UNINSTALL      = 0x4,
      TRANSMIT       = 0x5,
      RECEIVE        = 0x6,
      SPEED          = 0x7,
      STATUS         = 0x8
    };
    using DriverStatus = ESP32TWAI::DriverStatus;

  private:
    static constexpr std::size_t RX_QUEUE_LENGTH_DEFAULT {5};
    static constexpr std::size_t TX_QUEUE_LENGTH_DEFAULT {5};
    DriverStatus _lastErrorFunction;
};

#endif
