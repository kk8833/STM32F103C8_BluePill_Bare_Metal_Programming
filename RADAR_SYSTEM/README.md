# STM32F103C8T6 Blue Pill Radar System
A bare metal embedded radar system built on the STM32F103C8T6 (Blue Pill) microcontroller. No HAL, no libraries — every peripheral is configured and controlled through direct memory-mapped register access.

This project implements a sweeping ultrasonic radar system. A servo motor sweeps 0° to 180° and back continuously. At each step an HC-SR04 ultrasonic sensor measures the distance to any object in front of it. Distance readings are transmitted to a PC via USART serial communication. An active low buzzer alerts when an object is detected within range.



## Hardware Components
| Microcontroller | STM32F103C8T6 (Blue Pill) | Main controller 
| Servo Motor | SG90 | 180° radar sweep 
| Ultrasonic Sensor | HC-SR04 | Distance measurement 
| Buzzer | Active Low Buzzer | Object detection alert 
| USB to TTL Adapter | CP2102 | Serial communication to PC 
| Programmer | ST-Link V2 | Flashing firmware 


## Pin Configuration

| Pin | Peripheral | Function 

| PA0 | TIM2_CH1 | Servo PWM Output 
| PA6 | GPIO Input | HC-SR04 Echo 
| PA9 | USART1 TX | Serial Transmit to CP2102 RX 
| PA10 | USART1 RX | Serial Receive from CP2102 TX 
| PB0 | GPIO Output | HC-SR04 Trig 
| PB1 | GPIO Output | Buzzer (Active LOW) 



### Peripherals Configured via Register Access

RCC — Reset and Clock Control
- APB2ENR — enables clocks for GPIOA, GPIOB, USART1, AFIO
- APB1ENR — enables clocks for TIM2, TIM3

GPIO — General Purpose Input Output
- GPIOA CRL — PA0 as AF Push-Pull (Servo PWM), PA6 as Input Floating (Echo)
- GPIOA CRH — PA9 as AF Push-Pull (USART TX), PA10 as Input Floating (USART RX)
- GPIOB CRL — PB0 and PB1 as GP Output Push-Pull 50MHz (Trig and Buzzer)
- BSRR register used for atomic set and reset of GPIO pins

TIM2 — Servo PWM
- PSC = 7 → 8MHz / 8 = 1MHz → 1µs per tick
- ARR = 19999 → 20ms period = 50Hz
- CCMR1 = 0x68 → PWM Mode 1, preload enabled
- CCER bit 0 → CH1 output enabled
- CCR1 range 1000–2000 → maps to 0°–180°

TIM3 — Ultrasonic Timing and Delay
- PSC = 7 → 1µs per tick
- ARR = 65535 → free running up to 65ms
- Used as microsecond ruler for both trig delay and echo pulse measurement
- TIM3_CNT reset and polled manually in software

USART1 — Serial Communication
- BRR = (52<<4)|1 → 9600 baud at 8MHz
- 8 data bits, no parity, 1 stop bit
- TX only used — distance and alert strings sent to PC terminal


## Sweep and Measurement

- Servo sweeps forward from CCR1 = 1000 to 2000 (0° to 180°)
- Servo sweeps back from CCR1 = 2000 to 1000 (180° to 0°)
- At each step a 10µs TRIG pulse is sent to HC-SR04
- Echo pulse width is measured using TIM3_CNT
- Distance calculated as:

Distance (cm) = Echo_pulse_us / 58

- 30ms timeout on echo polling prevents system hang on missing echo

## Object Detection

- If distance > 2cm and <= 60cm → Buzzer ON, "Object Detected!" sent over USART
- If distance outside range → Buzzer OFF
- Buzzer is active low — PB1 LOW turns it ON, PB1 HIGH turns it OFF



## Clock Configuration

Running on internal HSI oscillator at **8MHz** — no PLL, no external crystal required. All timer and baud rate calculations are based on 8MHz system clock.



## What Makes This Bare Metal
- Zero HAL function calls
- Zero CMSIS peripheral structs
- All registers accessed as raw volatile uint32_t pointers
- Manual bit manipulation using shifts and masks for every config
- No RTOS, no interrupts — pure polling based execution
