# UART Communication with PC via CP2102 - Bare Metal

This project demonstrates bare-metal USART (Universal Synchronous/Asynchronous Receiver Transmitter) communication between the STM32F103C8T6 (Blue Pill) microcontroller and a PC using the CP2102 USB-to-UART converter module.

USART communication is widely used in embedded systems for:
- Serial debugging
- Data transmission
- PC-to-microcontroller interfacing

Since modern PCs do not provide direct UART interfaces, the CP2102 module is used to convert USB communication from the PC into UART signals compatible with the STM32 microcontroller.

This implementation currently focuses on asynchronous serial communication using:
- PA9  → USART1_TX
- PA10 → USART1_RX

## Concepts Used
- USART Asynchronous Communication
- Serial Data Transmission
- GPIO Alternate Function Configuration
- Baud Rate Configuration
- TX and RX Communication
- 
## Hardware Used

- STM32F103C8T6 Blue Pill
- CP2102 USB-to-UART Converter
- ST-Link V2 Programmer

## Peripheral Used

- USART1
- GPIOA (PA9 and PA10)
