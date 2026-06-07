 Multi-Node CAN Bus Sensor Networking & Diagnostics

This repository contains a low-level, register-level bare-metal implementation of a two-node Controller Area Network (CAN) designed for the STM32F103C8T6 (Blue Pill) microcontrollers running on an 8MHz system clock.

The system distributes sensory information across an isolated bus line to provide  using real-time peripherals without relying on high-level Hardware Abstraction Layers (HAL).


## 1. System Overview

The architecture divides operational load across two distinct processing modules configured to converse over a 125 kbps CAN bus interface:

Node 1: Receiver & Display Hub

Core Functions: Listens to the CAN bus frames, extracts telemetry payload data, and dynamically renders the layout to an SSD1306 OLED interface.
Peripherals Utilized: CAN1 (remapped to PB8/PB9), I2C1 (PB6/PB7 for OLED driving), and GPIO logic.
Node 2: Environmental Telemetry & Transmit Node

Core Functions: Interfaces with a Bosch BMP280 environmental sensor over I2C, runs real-time calibration/compensation logic, outputs local parameters via USART1, and frames data for CAN transmission.
Peripherals Utilized:CAN1 (remapped to PB8/PB9), I2C1 (PB6/PB7 for Sensor), USART1 (PA9 for terminal monitoring), and GPIO indicators.






## 3. Engineering Log & Debugging Journey

This implementation marks the culmination of an intensive hardware-software iteration process. The chronological debugging logs serve as an architectural trace of the issues encountered:

[Phase 1: Initial Deployments]
  - Symptom: OLED continuously displays "TEMP: 0 C | PRES: 0 hPa".
  - Attempts: ~50 iterative code uploads and hardware resets. 
  - Diagnosis: Bus communication failure due to a foundational bit-timing mismatch. Node 1 
               was executing at a default 72MHz clock configuration profile while Node 2 
               operated at 8MHz, creating asynchronous frame dropping.

[Phase 2: Unified Clock Alignments & Bit-Rate Normalization]
  - Changes: Modified registers to synchronize both microcontrollers to an identical 125 kbps rate.
  - Symptom: Display shifted to show "TEMP: 109 C | PRES: 42915515 hPa".
  - Verdict: EXTREMELY SIGNIFICANT. This erratic data proved that the physical CAN Bus link 
             was fully established and receiving packets. However, the data corruption 
             pointed to unaligned byte mapping or erroneous sensor calibration readings.

[Phase 3: The Mailbox Misalignment Trap]
  - Changes: Reworked the communication structures and re-audited memory layout maps.
  - Symptom: Values instantly read exactly "TEMP: 26 C". 
  - Diagnosis: The 26°C reading was accurate to room ambient temperature, but it was revealed 
               to be a structural fluke caused by an unintentional code typo cross-assigning 
               the high/low data registers (`CAN_RDH0R` and `CAN_RDL0R`), causing pressure data 
               to write directly into the temperature profile variables.

[Phase 4: Current State & Regression to Zero]
  - Changes: Corrected the register macro assignments to cleanly separate Temp and Pressure fields.
  - Symptom: The system regressed back to displaying "0 0".


 4. Current Root Cause Analysis

Because correcting valid software syntax and register routing logic caused the network data to vanish completely, the current state analysis isolates the problem to a Physical Layer / Hardware Instability rather than software logic.


