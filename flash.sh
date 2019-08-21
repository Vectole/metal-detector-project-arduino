#!/bin/bash
arm-none-eabi-objcopy -O binary .pio/build/genericSTM32F103C8/firmware.elf stm32-project.bin
stm32flash -w stm32-project.bin -v -g 0x08000000 /dev/ttyUSB0
