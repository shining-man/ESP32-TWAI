# Arduino Library for the ESP32 TWAI (CAN Bus)

This is a library to use the TWAI[^1] (CAN Bus) interface of the ESP32.
The library did not use the registry commands to configure the TWAI peripheral, but used the functions from the Espressif IDF to configure the TWAI peripheral, which are more reliable.

## Features
* Easily configure the ESP32 TWAI peripheral
* Various bus speeds
* Standard (11bit) and extended (29bit) frames
* Send and receive CAN Bus messages

## Usage
Starting the TWAI periphery with default settings:<br>
```C++
CAN.begin(CAN_GPIO_RX, CAN_GPIO_TX, can::Baudrate::BAUD_500KBPS)
```

or with custom settings:<br>
```C++
constexpr std::gpio_num_t CAN_GPIO_RX {GPIO_NUM_5};
constexpr std::gpio_num_t CAN_GPIO_TX {GPIO_NUM_4};
constexpr bool CAN_ENABLE_ALERTS {true};
constexpr std::size_t CAN_RX_QUEUE_LENGTH {10};
constexpr std::size_t CAN_TX_QUEUE_LENGTH {10};

CAN.begin(CAN_GPIO_RX,
          CAN_GPIO_TX,
          can::Baudrate::BAUD_500KBPS,
          CAN_ENABLE_ALERTS,
          CAN_RX_QUEUE_LENGTH,
          CAN_TX_QUEUE_LENGTH);
```

Send Message<br>
```C++
CAN.write(esp32::can::FrameType::STD_FRAME, identifier, length, buffer)
```

## Espressif IDF Documentation
The Espressif IDF documentation at the link below should be reference first if you encounter any errors. Make sure you are using the most recent IDF version when reading this.

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html


[^1]: TWAI is the name that Esperssif uses for the CAN peripheral. It stands for *two wire automotive interface*
