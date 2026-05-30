 STM32F103C8 I2C OLED Display Interfacing 
## Project Overview

Bare-metal C driver for displaying text on a 128x64 SSD1306 OLED display using the STM32F103C8T6 (Blue Pill) microcontroller over I2C, with no HAL or external libraries.

## Hardware Requirements

- STM32F103C8T6 (Blue Pill)
- SSD1306 128x64 OLED (I2C version)
- 3.3V power supply
- ST-Link V2 programmer

## Pin Connections
STM32 Pin
| PB6 | SCL | I2C1 Clock 
| PB7 | SDA | I2C1 Data 





## Register Configuration
- PB6 and PB7 set to Alternate Function Open-Drain, 50MHz mode


I2C Peripheral
- APB1 clock: 8 MHz (I2C_CR2 = 0x08)
- Standard mode 100kHz (I2C_CCR = 0x28)
- Rise time register: I2C_TRISE = 0x09

## OLED Initialization Sequence

| Command | Purpose |

| 0xAE | Display OFF |
| 0xA1 | Segment remap |
| 0xC8 | COM scan direction |
| 0xA8, 0x3F | Multiplex ratio (64 rows) |
| 0xD3, 0x00 | Display offset zero |
| 0xDA, 0x12 | COM pin config |
| 0x8D, 0x14 | Charge pump enable |
| 0x20, 0x02 | Page addressing mode |
| 0xAF | Display ON |


## I2C Communication Flow

START → Address(0x3C, Write) → Control byte → Data byte(s) → STOP

Command:  START → 0x3C → 0x00 → cmd_byte  → STOP
Data:     START → 0x3C → 0x40 → data_byte → STOP



## Addressing Mode

Page addressing mode (0x20, 0x02) is used. Cursor is set with:


OLED_cmd(0xB0 + page);        // Page 0–7
OLED_cmd(0x00 + (col & 0x0F)); // Column lower nibble
OLED_cmd(0x10 + (col >> 4));   // Column upper nibble

## Font Format

5x7 pixel font, one byte per column, LSB at top MSB at bottom.

---

## Tested On

- STM32F103C8T6 Blue Pill
- 0.96" SSD1306 I2C OLED (128x64)
- Proteus 8 simulation
- 8MHz internal HSI clock
