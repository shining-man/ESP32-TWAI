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
  // Make this class non-copyable, because it`s a singleton
  public:
    ESP32TWAI(const ESP32TWAI& other) = delete;
    ESP32TWAI& operator=(const ESP32TWAI& other) = delete;

  public:
    /** Returns the singleton instance (static) of the ESP32TWAI. */
    static ESP32TWAI& getInstance();

    esp_err_t   begin(gpio_num_t rxPin, gpio_num_t txPin, TWAI_speed_s baud);
    void        setAlert(bool alert);
    void        setRxQueueLen(std::size_t rxQueueLen);
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
    // This is a Singleton, we do not allow to create an instance from outside the class!
    ESP32TWAI();
    ~ESP32TWAI();

  private:
    static constexpr std::size_t RX_QUEUE_LENGTH_DEFAULT = 5;
    static constexpr std::size_t TX_QUEUE_LENGTH_DEFAULT = 10;
    bool _alertsEnabled =false;
    DriverStatus _lastErrorFunction;
    std::size_t _rxQueueLen;
};

/**
 * The following #define can be used to access the singleton object of the ESP32TWAI.
 * The getInstance() methods takes care to create the object on the first call.
 * @code
 * CAN.begin(GPIO_NUM_5,GPIO_NUM_4,TWAI_SPEED_250KBPS);
 * twai_status_info_t canStatus = CAN.getStatus();
 * @endcode
 *
*/
#define CAN (ESP32TWAI::getInstance())

#endif


