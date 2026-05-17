
# SysTick Interrupt Handler - Bare Metal

This project demonstrates a bare-metal implementation of the ARM Cortex-M3 SysTick timer using the SysTick interrupt handler on the STM32F103C8T6 (Blue Pill) microcontroller for periodic LED blinking.

The STM32 Blue Pill contains multiple hardware timers including advanced-control and general-purpose timers. In addition to these peripheral timers, the ARM Cortex-M3 core itself provides an internal 24-bit system timer known as the SysTick Timer.

In this implementation, the SysTick timer generates periodic interrupts, and the LED toggling operation is handled inside the `SysTick_Handler()` interrupt service routine.

The SysTick timer is commonly used for:
- Periodic interrupts
- Time base generation
- RTOS scheduling
- System tick management
- Non-blocking timing operations
