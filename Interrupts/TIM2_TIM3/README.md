TIM2 and TIM3 Interrupt Based LED Blinking – Bare Metal STM32

This project demonstrates the implementation of TIM2 and TIM3 interrupts on the STM32F103C8 microcontroller using bare-metal programming without HAL libraries.
The main objective was to understand how hardware interrupts work internally compared to traditional polling-based methods.

This was my first implementation  using interrupts instead of continuously checking flags in a loop (polling). These helped understand the working of the NVIC (Nested Vector Interrupt Controller), timer peripherals, ISR execution flow, and memory-mapped register programming.


References Used
PM0056 Cortex-M3 Programming Manual
RM0008 STM32 Reference Manual
STM32F100C8 Datasheet

The project was implemented by manually configuring peripheral registers through their mapped memory addresses based on the documentation provided in these manuals.

Working Principle
TIM2 and TIM3 timers are configured with suitable prescaler and auto-reload values.
Timer update interrupts are enabled.
NVIC is configured to allow the corresponding timer interrupts.
When the timer overflows, the CPU automatically jumps to the ISR.
Inside the ISR, the LED state is toggled.
This removes the need for continuously checking timer flags in the main loop.


