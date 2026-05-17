
# Seven Segment Display Interfacing - Bare Metal

This is the bare-metal interfacing of a Seven Segment Display with the STM32F103C8T6 (Blue Pill) microcontroller using GPIO Port B.

The implementation uses direct register-level programming and memory-mapped peripheral access without relying on HAL libraries or external driver frameworks.

The GPIO pins of Port B are configured as digital outputs to control the individual segments (`a` to `g`) of the Seven Segment Display. Different bit patterns are written to the GPIO output register to display numerical digits.

This project helps in understanding:
- GPIO output configuration
- Bit-level manipulation
- Peripheral register control

- GPIO Port Configuration
- GPIO Output Data Register (ODR)
- Bit Masking
- Register-Level Programming
- Bare-Metal Embedded C
- Memory-Mapped I/O





## Result

Successfully interfaced and controlled a Seven Segment Display using GPIO Port B on the STM32F103C8T6 through bare-metal embedded programming.
