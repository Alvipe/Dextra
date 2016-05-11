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

To send position commands to each finger individually, connect the board to a USB port of your computer and use the send_position.py script:

```bash
~/Dextra$ cd Firmware
~/Dextra/Firmware$ python send_position.py
