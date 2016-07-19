[![Build Status](https://travis-ci.org/Alvipe/Dextra.svg?branch=master)](https://travis-ci.org/Alvipe/Dextra)

To upload the firmware to one of the supported boards, use [platformio](http://platformio.org/).

For Teensy 3.1 (Arduino board):
```bash
~/Dextra$ cd Firmware/Teensy31
~/Dextra/Firmware/Teensy31$ platformio run --target upload
```

For STM32F411 Nucleo (mbed board):
```bash
~/Dextra$ cd Firmware/STM32F411Nucleo
~/Dextra/Firmware/STM32F411Nucleo$ platformio run --target upload
```
