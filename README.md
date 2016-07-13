stm32-unicore-mx
================

This repository contains the example code for our tutorial series on
programming STM32 microcontrollers using [unicore-mx](https://github.com/insane-adding-machines/unicore-mx).

### Prerequisites:
- arm-embedded-gcc toolchain in PATH
- st-flash (texane) in PATH
- unicore-mx in UCMX_DIR
- PA9 pin (USART1 TX) connected to a USART RX

### Building:
Each example must be built from its own directory. You can build and flash in one move using:
`make flash`

Once an example is flashed, you can use screen/minicom at 115200 baudrate to connect to the USART.
