Default I2C Pins Devkit M1 SDA: 8 SCL: 9
How to custom: https://randomnerdtutorials.com/esp32-i2c-communication-arduino-ide/
Changed to 6&7 so on-board LED is off

TFT Bugfix

.pio\libdeps\esp32-c3-devkitm-1\TFT_eSPI\Processors\TFT_eSPI_ESP32_C3.h

Line 76

// ESP32 specific SPI port selection - only SPI2_HOST available on C3
#if ESP_ARDUINO_VERSION_MAJOR < 3
  #define SPI_PORT 2 //SPI2_HOST
#else
  #define SPI_PORT 2
#endif