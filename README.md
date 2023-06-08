# Arduino Library for the ESP32 TWAI (CAN Bus)

This is a library to use the TWAI[^1] (CAN Bus) interface of the ESP32. 
The library did not use the registry commands to configure the TWAI peripheral, but used the functions from the Espressif IDF to configure the TWAI peripheral, which are more reliable. 

## Features
* Easily configure the ESP32 TWAI peripheral
* Various bus speeds
* Standard (11bit) and extended (29bit) frames
* Send and receive CAN Bus messages

## Usage
Starting the TWAI periphery
´CAN.begin(GPIO_NUM_5,GPIO_NUM_4,TWAI_SPEED_500KBPS)´

Send Message
´CAN.write(TWAI_STD_FRAME,identifier,length,buffer)´

## Espressif IDF Documentation
The Espressif IDF documentation at the link below should be reference first if you encounter any errors. Make sure you are using the most recent IDF version when writing this. 

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/twai.html


[^1]: TWAI is the name that Esperssif uses for the CAN peripheral. It stands for *two wire automotive interface*