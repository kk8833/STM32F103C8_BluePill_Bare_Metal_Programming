OLED Receiver Hub (Bare-Metal CAN & I2C)

This section of the repository contains the low-level, register-level bare-metal implementation for Node 1 (The OLED Receiver Hub) running on an 8MHz system clock.

Unlike the transmission side, the software and hardware stacks on this specific node are fully validated and operating perfectly. The custom bit-banged register-level I2C driver successfully initializes, clears, and updates the SSD1306 display terminal with zero dependency on external HAL utility libraries.


## 1. Node Operational Status

CAN Bus Core Layer:Ready. Configured for a stable 125 kbps bus speed to match the 8MHz peripheral clock network.
I2C Peripheral Configuration:Fully Successful.Direct register tracking of the Control, Status, and Data registers (I2C_CR1, I2C_SR1, I2C_DR) successfully executes Start, Stop, and Address ACK handling sequences.
Display Output :Fully Successful. The custom 5x7 font arrays map seamlessly onto the display columns. The screen initializes cleanly and drops into its telemetry display layout on bootup without freezing.




### Core I2C Initialization & Control


void I2C_init() {
    I2C_CR1 &= ~(1 << 0); // Disable peripheral for configuration
    I2C_CR2 = 0x08;        // Set APB1 Clock Frequency to 8MHz
    I2C_CCR = 0x28;        // Configure communication speed (Standard Mode)
    I2C_TRISE = 0x09;      // Program maximum rise time parameters
    I2C_CR1 |= (1 << 0);  // Enable I2C peripheral module
}






