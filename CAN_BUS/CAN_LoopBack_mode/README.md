# CAN Loopback — STM32F103C8 Blue Pill
### Bare Metal | No HAL | Direct Register Access

## Project Overview

Bare metal implementation of CAN loopback mode on the STM32F103C8T6 (Blue Pill)
microcontroller. Written entirely from scratch using direct register access — no HAL,
no libraries, no abstraction layers. Demonstrates successful CAN frame transmission
and reception through the internal bxCAN peripheral loopback mechanism.


## What This Project Demonstrates

- Manual bxCAN peripheral initialization from reset state
- Bit timing calculation from first principles
- TX mailbox loading and transmission triggering
- RX FIFO polling and message extraction
- Filter bank configuration in 32-bit mask mode
- Full CAN loopback round trip verification via ST-Link debugger


No transceiver or external CAN bus required for this test.
The bxCAN peripheral internally loops TX back to RX in software.


## CAN Configuration

| Parameter | Value 

| Mode | Loopback (LBKM = 1) |
| APB1 Clock | 8 MHz 
| Baud Rate | 500 kbps 
| Prescaler (BRP) | 0 (prescaler = 1) 
| Time Segment 1 | 11 TQ (register value 10) 
| Time Segment 2 | 4 TQ (register value 3) 
| SJW | 1 TQ (register value 0) 
| Total TQ per bit | 16 
| Sample Point | 75% 
| Frame Format | Standard 11-bit ID (CAN 2.0A) 
| Message ID | 0x123 
| DLC | 8 bytes 

### Bit Timing Calculation


APB1 = 8 MHz  →  one clock cycle = 125 ns
Prescaler = 1  →  TQ = 125 ns
Total TQ = 1 (Sync) + 11 (TS1) + 4 (TS2) = 16 TQ
Baud = 8,000,000 / (1 × 16) = 500,000 bps = 500 kbps
Sample point = (1 + 11) / 16 = 75%


## Pin Configuration

| Pin 

| PA12 | CAN1_TX (default mapping) | Alternate Function Push-Pull 50MHz 
| PA11 | CAN1_RX (default mapping) | Input Floating 



## Register Map Used

### Control Registers

| Register | Address | Purpose |

| RCC_APB2ENR | 0x40021018 | Enable GPIOA clock |
| RCC_APB1ENR | 0x4002101C | Enable CAN1 clock |
| GPIOA_CRH | 0x40010804 | Configure PA11/PA12 |
| CAN_MCR | 0x40006400 | Master control |
| CAN_MSR | 0x40006404 | Master status |
| CAN_BTR | 0x4000641C | Bit timing + loopback mode |

### Filter Registers

| Register | Address | Purpose |
| CAN_FMR | 0x40006600 | Filter master (FINIT) |
| CAN_FM1R | 0x40006604 | Filter mode (mask) |
| CAN_FS1R | 0x4000660C | Filter scale (32-bit) |
| CAN_FFA1R | 0x40006614 | Filter FIFO assignment |
| CAN_FA1R | 0x4000661C | Filter activation |
| CAN_F0R1 | 0x40006640 | Filter bank 0 ID |
| CAN_F0R2 | 0x40006644 | Filter bank 0 mask |

### TX Mailbox 0 Registers

| Register | Address | Purpose |

| CAN_TI0R | 0x40006580 | TX identifier + TXRQ |
| CAN_TDT0R | 0x40006584 | TX data length (DLC) |
| CAN_TDL0R | 0x40006588 | TX data bytes 0-3 |
| CAN_TDH0R | 0x4000658C | TX data bytes 4-7 |
| CAN_TSR | 0x40006408 | TX status (TXOK0, TME0) |

### RX FIFO 0 Registers

| Register | Address | Purpose |

| CAN_RF0R | 0x4000640C | FIFO status + release |
| CAN_RI0R | 0x400065B0 | RX identifier |
| CAN_RDT0R | 0x400065B4 | RX data length |
| CAN_RDL0R | 0x400065B8 | RX data bytes 0-3 |
| CAN_RDH0R | 0x400065BC | RX data bytes 4-7 |


## Initialization Sequence


1.  RCC_APB2ENR  →  enable GPIOA clock
2.  RCC_APB1ENR  →  enable CAN1 clock
3.  GPIOA_CRH    →  PA12 AF_PP, PA11 Input Float
4.  CAN_MCR      →  INRQ=1, ABOM=1, SLEEP=0
5.  CAN_MSR      →  poll until INAK=1, SLAK=0
6.  CAN_BTR      →  BRP=0, TS1=10, TS2=3, SJW=0, LBKM=1
7.  CAN_FMR      →  FINIT=1 (unlock filters)
8.  CAN_FA1R     →  deactivate bank 0
9.  CAN_FM1R     →  mask mode for bank 0
10. CAN_FS1R     →  32-bit scale for bank 0
11. CAN_FFA1R    →  assign bank 0 to FIFO0
12. CAN_F0R1     →  ID = 0x123
13. CAN_F0R2     →  mask = 0x7FF (exact match)
14. CAN_FA1R     →  activate bank 0
15. CAN_FMR      →  FINIT=0 (lock filters)
16. CAN_MCR      →  INRQ=0 (leave init mode)
17. CAN_MSR      →  poll until INAK=0

## Test Message

| Field | Value 
| Message ID | 0x123 
| DLC | 8 bytes 
| Payload | K R I S H N A L 
| Payload (hex) | 4B 52 49 53 48 4E 41 4C 
| TDL0R | 0x5349524B 
| TDH0R | 0x4C414E48 

## Test Result

Verified via ST-Link debugger, breakpoint placed at RFOM0 release line.
Variables inspected after reception:

| Variable | Expected | Actual | Result 

| check_rx_id | 0x123 | 0x123 
| RX_Low_byte | 0x5349524B | 0x5349524B 
| RX_High_byte | 0x4C414E48 | 0x4C414E48 

All 8 bytes received correctly. Message ID verified. CAN loopback confirmed working.




## Next Steps

| Stage | Description | Hardware Needed 

| Stage 2 | Normal mode, two nodes | 2x Blue Pill, 2x SN65HVD230 |
| Stage 3 | Interrupt driven RX | Same as Stage 2 
| Stage 4 | Three nodes, arbitration demo | 3x Blue Pill, 3x SN65HVD230 


## References

- STM32F103C8 Reference Manual (RM0008) — bxCAN chapter
- STM32F103C8 Datasheet — pin definitions and alternate functions
