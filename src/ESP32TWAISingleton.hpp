// Copyright (c) 2024 Meik Jaeckle
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef INC_ESP32TWAISINGLETON_H
#define INC_ESP32TWAISINGLETON_H

#include <ESP32TWAI.h>

namespace esp32
{
namespace can
{

/**
 * Provide a simple singleton wrapper for the ESP32TWAI class to support legacy code,
 * where global access to the ESP32TWAI class is required.
 * @note If possible, don´t use the singleton, use some kind of dependency injection instead.
 * The #define "CAN" can be used as an "alias" to access the singleton object of the ESP32TWAI.
 * @code
 * CAN.begin(GPIO_NUM_5,GPIO_NUM_4,TWAI_SPEED_250KBPS);
 * twai_status_info_t canStatus = CAN.getStatus();
 * @endcode
 *
*/
class ESP32TWAISingleton :
  public ESP32TWAI
{
  private:
    // don't allow public construct/destruction
    ESP32TWAISingleton()  = default;
    ~ESP32TWAISingleton() = default;

  // Make this class non-copyable, it`s a singleton
  public:
    ESP32TWAISingleton(const ESP32TWAISingleton& other) = delete;
    ESP32TWAISingleton& operator=(const ESP32TWAISingleton& other) = delete;

    /** Returns the singleton instance (static) of the ESP32TWAISingleton.
     *  The instance is created on the first call to getInstance.
    */
    static ESP32TWAISingleton& getInstance()
    {
      static ESP32TWAISingleton instance;
      return instance;
    }
};

} // namespace can
} // namespace esp32

#define CAN (esp32::can::ESP32TWAISingleton::getInstance())

#endif // INC_ESP32TWAISINGLETON_H
