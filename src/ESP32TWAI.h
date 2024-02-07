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

namespace can
{

enum class Baudrate
{
  BAUD_100KBPS = 100,
  BAUD_125KBPS = 125,
  BAUD_250KBPS = 250,
  BAUD_500KBPS = 500,
  BAUD_800KBPS = 800,
  BAUD_1MBPS   = 1000
};

enum class FrameType
{
  STD_FRAME  = 0,
  EXTD_FRAME = 1
};

class ESP32TWAI
{
  public:
    ESP32TWAI();
    ~ESP32TWAI();

  public:
    esp_err_t begin(gpio_num_t rxPin,
                    gpio_num_t txPin,
                    Baudrate baud,
                    bool enableAlerts = false,
                    std::size_t rxQueueLength = RX_QUEUE_LENGTH_DEFAULT,
                    std::size_t txQueueLength = TX_QUEUE_LENGTH_DEFAULT);
    esp_err_t   stop();
    esp_err_t   write(FrameType extd, uint32_t identifier, uint8_t length, uint8_t *buffer);
    esp_err_t   read(twai_message_t* ptr_message);
    uint32_t    getAlert() const;
    twai_status_info_t getStatus() const;
    std::string getErrorText(esp_err_t errNo) const;

  private:
    enum class DriverStatus : uint8_t
    {
      NONE           = 0x0,
      DRIVER_INSTALL = 0x1,
      START          = 0x2,
      STOP           = 0x3,
      UNINSTALL      = 0x4,
      TRANSMIT       = 0x5,
      RECEIVE        = 0x6,
      SPEED          = 0x7,
      STATUS         = 0x8
    };

  private:
    static constexpr std::size_t RX_QUEUE_LENGTH_DEFAULT {5};
    static constexpr std::size_t TX_QUEUE_LENGTH_DEFAULT {5};
    DriverStatus _lastErrorFunction;
};

} // namespace can

#endif
