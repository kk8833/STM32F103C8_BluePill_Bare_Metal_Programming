# STM32 Blue Pill Radar System — Interrupt Driven
An upgraded bare metal embedded radar system built on the STM32F103C8T6 (Blue Pill) microcontroller. This version replaces the polling based echo measurement with TIM3 Input Capture interrupt driven architecture. No HAL, no libraries — every peripheral is configured through direct memory-mapped register access.



## Overview
A servo motor continuously sweeps 0° to 180° and back. At each step an HC-SR04 ultrasonic sensor fires a TRIG pulse and the ECHO response is captured using TIM3 Input Capture interrupt. The ISR measures the pulse width by toggling between rising and falling edge capture. Distance is calculated and transmitted to a PC via USART1 serial. An active low buzzer alerts when an object is detected within range.



## Hardware Components
| Microcontroller | STM32F103C8T6 Blue Pill | Main controller
| Servo Motor | SG90 | 180° radar sweep 
| Ultrasonic Sensor | HC-SR04 | Distance measurement
| Buzzer | Active Low Buzzer | Object detection alert 
| USB to TTL Adapter | CP2102 | Serial communication to PC 


## Pin Configuration

| Pin | Peripheral | Function |

| PA0 | TIM2_CH1 | Servo PWM Output |
| PA6 | TIM3_CH1 | HC-SR04 Echo — Input Capture |
| PA9 | USART1 TX | Serial Transmit to CP2102 RX |
| PA10 | USART1 RX | Serial Receive from CP2102 TX |
| PB0 | GPIO Output | HC-SR04 Trig |
| PB1 | GPIO Output | Buzzer Active LOW |


### Peripherals Configured via Register Access

**RCC — Reset and Clock Control**
- APB2ENR — enables GPIOA, GPIOB, USART1, AFIO clocks
- APB1ENR — enables TIM2, TIM3 clocks

**GPIO**
- GPIOA CRL — PA0 AF Push-Pull 50MHz (Servo PWM), PA6 Input Floating (Echo IC)
- GPIOA CRH — PA9 AF Push-Pull 50MHz (USART TX), PA10 Input Floating (USART RX)
- GPIOB CRL — PB0 and PB1 GP Output Push-Pull 50MHz (Trig and Buzzer)
- BSRR used for atomic GPIO set and reset

**TIM2 — Servo PWM**
- PSC = 7 → 8MHz / 8 = 1MHz → 1µs per tick
- ARR = 19999 → 20ms period = 50Hz
- CCMR1 = 0x68 → PWM Mode 1, preload enabled
- CCR1 range 1000 to 2000 → maps 0° to 180°
- Sweep step = 10 CCR1 counts = ~1.8° per step

**TIM3 — Input Capture + Delay**
- PSC = 7 → 1µs per tick
- ARR = 65535 → free running up to 65ms
- CCMR1 CC1S = 01 → Input Capture on TI1 (PA6)
- DIER CC1IE = 1 → capture interrupt enabled
- CCER CC1P toggled in ISR between rising and falling edge
- Also used as microsecond ruler for TRIG delay via TIM3_CNT polling

**USART1 — Serial Communication**
- BRR = (52<<4)|1 → 9600 baud at 8MHz
- 8 data bits, no parity, 1 stop bit
- TX polling based — TXE flag checked before each byte

**NVIC — Nested Vector Interrupt Controller**
- IPR7 bits[15:8] → TIM3 IRQ29 priority 0 (highest)
- ISER0 bit 29 → TIM3 IRQ29 enabled


## Interrupt Driven Echo Measurement

The key upgrade over the polling version. TIM3 CH1 captures PA6 Echo pin transitions automatically:

TRIG fired from main
        ↓
Echo goes HIGH → TIM3 capture interrupt fires
ISR saves rise_time = TIM3_CCR1
ISR flips CCER to falling edge
        ↓
Echo goes LOW → TIM3 capture interrupt fires again
ISR calculates Echo_Pulse = TIM3_CCR1 - rise_time
ISR flips CCER back to rising edge
ISR sets data_ready = 1
        ↓
Main sees data_ready → transmits Angle and Distance


### Edge Toggle State Machine in ISR


if(edge_state == 0)             // rising edge captured
{
    rise_time  = TIM3_CCR1;
    TIM3_CCER |= (1<<1);        // switch to falling edge
    edge_state = 1;
}
else                            // falling edge captured
{
    Echo_Pulse = TIM3_CCR1 - rise_time;
    Distance   = Echo_Pulse / 58;
    TIM3_CCER &= ~(1<<1);       // switch back to rising edge
    edge_state = 0;
    data_ready = 1;
}

## Distance Calculation

Distance (cm) = Echo_Pulse (µs) / 58

Derived from:
Speed of sound = 343 m/s = 0.0343 cm/µs
Distance = (Echo_Pulse × 0.0343) / 2 = Echo_Pulse / 58.3 ≈ Echo_Pulse / 58
Division by 2 accounts for sound travelling to object and back
Integer division used — 1cm resolution sufficient for radar application

## Sweep and Measurement Flow

- Servo sweeps forward CCR1 1000 to 2000 in steps of 10 (0° to 180°)
- Servo sweeps reverse CCR1 2000 to 1000 in steps of 10 (180° to 0°)
- At each step 15ms settle delay then 10µs TRIG pulse fired
- Main waits on data_ready flag with 50000 iteration timeout
- Angle and distance transmitted as comma separated values

## Object Detection

- Distance > 2cm and <= 60cm → Buzzer ON, data sent over USART
- Distance outside range → Buzzer OFF
- Active LOW buzzer — PB1 LOW turns ON, PB1 HIGH turns OFF





## Clock Configuration

Internal HSI oscillator at 8MHz 


## Upgrade from Polling Version

| Feature | Polling Version | Interrupt Version 

| Echo measurement | Busy wait on GPIOA_IDR | TIM3 Input Capture ISR |
| CPU during echo wait | Blocked | Free |
| Edge detection | Manual while loops | Hardware capture + ISR toggle |
| Distance update | Inline in main | Via volatile shared variables |
| Timeout | TIM3_CNT check in while | 50000 iteration counter in main |



