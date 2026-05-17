# SysTick Timer - Bare Metal

This project demonstrates a basic bare-metal implementation of the SysTick timer on the STM32F103C8T6 (Blue Pill) microcontroller for generating delay-based LED blinking.

The STM32 Blue Pill contains multiple hardware timers including advanced-control and general-purpose timers. Along with these peripheral timers, the ARM Cortex-M3 core itself also provides an internal 24-bit system timer known as the SysTick Timer.

The SysTick timer is tightly integrated within the Cortex-M3 processor core and is commonly used for:
- Time delays
- Periodic interrupts
- RTOS task scheduling
- System time base generation

This implementation uses direct register-level programming and memory-mapped access without relying on HAL libraries.




