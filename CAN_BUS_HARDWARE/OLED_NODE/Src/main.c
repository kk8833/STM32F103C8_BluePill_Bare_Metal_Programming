#include <stdint.h>

// Clock enable
#define RCC_APB2ENR (*(volatile uint32_t*)0x40021018)
#define RCC_APB1ENR (*(volatile uint32_t*)0x4002101C)

// Port A
#define GPIOA_CRL (*(volatile uint32_t*)0x40010800)
#define GPIOA_IDR (*(volatile uint32_t*)0x40010808)
#define GPIOA_ODR (*(volatile uint32_t*)0x4001080C)

// PORT B
#define PORTB_CRL (*(volatile uint32_t*)0x40010C00)
#define PORTB_CRH (*(volatile uint32_t*)0x40010C04)

// PIN remap for CAN
#define AFIO_MAPR (*(volatile uint32_t*)0x40010004)

// I2C - OLED
#define I2C_CR1   (*(volatile uint32_t*)0x40005400)
#define I2C_CR2   (*(volatile uint32_t*)0x40005404)
#define I2C_DR    (*(volatile uint32_t*)0x40005410)
#define I2C_SR1   (*(volatile uint32_t*)0x40005414)
#define I2C_SR2   (*(volatile uint32_t*)0x40005418)
#define I2C_CCR   (*(volatile uint32_t*)0x4000541C)
#define I2C_TRISE (*(volatile uint32_t*)0x40005420)

// CAN registers
#define CAN_MCR   (*(volatile uint32_t*)0x40006400)
#define CAN_MSR   (*(volatile uint32_t*)0x40006404)
#define CAN_BTR   (*(volatile uint32_t*)0x4000641C)
#define CAN_TSR   (*(volatile uint32_t*)0x40006408)
#define CAN_RF0R  (*(volatile uint32_t*)0x4000640C)

#define CAN_FMR   (*(volatile uint32_t*)0x40006600)
#define CAN_FM1R  (*(volatile uint32_t*)0x40006604)
#define CAN_FS1R  (*(volatile uint32_t*)0x4000660C)
#define CAN_FFA1R (*(volatile uint32_t*)0x40006614)
#define CAN_FA1R  (*(volatile uint32_t*)0x4000661C)
#define CAN_F0R1  (*(volatile uint32_t*)0x40006640)
#define CAN_F0R2  (*(volatile uint32_t*)0x40006644)

// Mailbox 0
#define CAN_TI0R  (*(volatile uint32_t*)0x40006580)
#define CAN_TDT0R (*(volatile uint32_t*)0x40006584)
#define CAN_TDL0R (*(volatile uint32_t*)0x40006588)

#define CAN_RI0R  (*(volatile uint32_t*)0x400065B0)
#define CAN_RDL0R (*(volatile uint32_t*)0x400065B8)
#define CAN_RDH0R (*(volatile uint32_t*)0x400065BC)

void GPIO_init() {
    RCC_APB2ENR |= (1 << 0) | (1 << 2) | (1 << 3);
    RCC_APB1ENR |= (1 << 21) | (1 << 25);


    PORTB_CRL &= ~(0xFF << 24);
    PORTB_CRL |=  (0xFF << 24);

    PORTB_CRH &= ~(0xFF << 0);
    PORTB_CRH |=  (0xB4 << 0);

    GPIOA_CRL &= ~(0xF << 0);
    GPIOA_CRL |=  (0x8 << 0);
    GPIOA_ODR |=  (1 << 0);

    AFIO_MAPR &= ~(0x3 << 13);
    AFIO_MAPR |=  (0x2 << 13);
}

void I2C_init() {
    I2C_CR1 &= ~(1 << 0);
    I2C_CR2 = 0x08;
    I2C_CCR = 0x28;
    I2C_TRISE = 0x09;
    I2C_CR1 |= (1 << 0);
}

void I2C_start() {
    I2C_CR1 |= (1 << 8);
    volatile uint32_t timeout = 5000;
    while (!(I2C_SR1 & (1 << 0)) && --timeout);
}

void I2C_stop() {
    volatile uint32_t timeout = 5000;
    while (!(I2C_SR1 & (1 << 7)) && --timeout);
    timeout = 5000;
    while (!(I2C_SR1 & (1 << 2)) && --timeout);
    I2C_CR1 |= (1 << 9);
}

void I2C_write(uint8_t data) {
    volatile uint32_t timeout = 5000;
    while (!(I2C_SR1 & (1 << 7)) && --timeout);
    I2C_DR = data;
}

void ADDR_flag() {
    volatile uint32_t timeout = 5000;
    while (!(I2C_SR1 & (1 << 1)) && --timeout);
    volatile uint32_t tmp = I2C_SR2;
    (void)tmp;
}

void OLED_cmd(uint8_t cmd) {
    I2C_start();
    I2C_DR = (0x3C << 1);
    ADDR_flag();
    I2C_write(0x00);
    I2C_write(cmd);
    I2C_stop();
}

void OLED_init() {
    OLED_cmd(0xAE); OLED_cmd(0xA1); OLED_cmd(0xC8); OLED_cmd(0xA8);
    OLED_cmd(0x3F); OLED_cmd(0xD3); OLED_cmd(0x00); OLED_cmd(0xDA);
    OLED_cmd(0x12); OLED_cmd(0x8D); OLED_cmd(0x14); OLED_cmd(0x20);
    OLED_cmd(0x02); OLED_cmd(0xAF);
}

void OLED_set_page_cursor(uint8_t page, uint8_t column) {
    OLED_cmd(0xB0 + (page & 0x07));
    OLED_cmd(0x00 + (column & 0x0F));
    OLED_cmd(0x10 + ((column >> 4) & 0x0F));
}

void OLED_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        OLED_set_page_cursor(page, 0);
        I2C_start();
        I2C_DR = (0x3C << 1);
        ADDR_flag();
        I2C_write(0x40);
        for (uint8_t col = 0; col < 128; col++) {
             I2C_write(0x00);
        }
        I2C_stop();
    }
}

const uint8_t Font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x5F, 0x00, 0x00},
    {0x00, 0x07, 0x00, 0x07, 0x00}, {0x14, 0x7F, 0x14, 0x7F, 0x14},
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, {0x23, 0x13, 0x08, 0x64, 0x62},
    {0x36, 0x49, 0x55, 0x22, 0x50}, {0x00, 0x05, 0x03, 0x00, 0x00},
    {0x00, 0x1C, 0x22, 0x41, 0x00}, {0x00, 0x41, 0x22, 0x1C, 0x00},
    {0x14, 0x08, 0x3E, 0x08, 0x14}, {0x08, 0x08, 0x3E, 0x08, 0x08},
    {0x00, 0x50, 0x30, 0x00, 0x00}, {0x08, 0x08, 0x08, 0x08, 0x08},
    {0x00, 0x60, 0x60, 0x00, 0x00}, {0x20, 0x10, 0x08, 0x04, 0x02},
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, {0x00, 0x42, 0x7F, 0x40, 0x00},
    {0x42, 0x61, 0x51, 0x49, 0x46}, {0x21, 0x41, 0x45, 0x4B, 0x31},
    {0x18, 0x14, 0x12, 0x7F, 0x10}, {0x27, 0x45, 0x45, 0x45, 0x39},
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, {0x01, 0x71, 0x09, 0x05, 0x03},
    {0x36, 0x49, 0x49, 0x49, 0x36}, {0x06, 0x49, 0x49, 0x29, 0x1E},
    {0x00, 0x36, 0x36, 0x00, 0x00}, {0x00, 0x56, 0x36, 0x00, 0x00},
    {0x08, 0x14, 0x22, 0x41, 0x00}, {0x24, 0x24, 0x24, 0x24, 0x24},
    {0x00, 0x41, 0x22, 0x14, 0x08}, {0x02, 0x01, 0x51, 0x09, 0x06},
    {0x32, 0x49, 0x79, 0x41, 0x3E}, {0x7E, 0x11, 0x11, 0x11, 0x7E},
    {0x7F, 0x49, 0x49, 0x49, 0x36}, {0x3E, 0x41, 0x41, 0x41, 0x22},
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, {0x7F, 0x49, 0x49, 0x49, 0x41},
    {0x7F, 0x09, 0x09, 0x09, 0x01}, {0x3E, 0x41, 0x49, 0x49, 0x7A},
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, {0x00, 0x41, 0x7F, 0x41, 0x00},
    {0x20, 0x40, 0x41, 0x3F, 0x01}, {0x7F, 0x08, 0x14, 0x22, 0x41},
    {0x7F, 0x40, 0x40, 0x40, 0x40}, {0x7F, 0x02, 0x0C, 0x02, 0x7F},
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, {0x3E, 0x41, 0x41, 0x41, 0x3E},
    {0x7F, 0x09, 0x09, 0x09, 0x06}, {0x3E, 0x41, 0x51, 0x21, 0x5E},
    {0x7F, 0x09, 0x19, 0x29, 0x46}, {0x46, 0x49, 0x49, 0x49, 0x31},
    {0x01, 0x01, 0x7F, 0x01, 0x01}, {0x3F, 0x40, 0x40, 0x40, 0x3F},
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, {0x3F, 0x40, 0x38, 0x40, 0x3F},
    {0x63, 0x14, 0x08, 0x14, 0x63}, {0x07, 0x08, 0x70, 0x08, 0x07},
    {0x61, 0x51, 0x4D, 0x43, 0x41}
};

void OLED_char(uint8_t word) {
    if (word >= 'a' && word <= 'z') word -= 32;
    if (word < 32 || word > 90) word = ' ';

    uint8_t font_index = word - 32;
    I2C_start();
    I2C_DR = (0x3C << 1);
    ADDR_flag();
    I2C_write(0x40);

    for (uint8_t col = 0; col < 5; col++) {
        volatile uint32_t timeout = 5000;
        while (!(I2C_SR1 & (1 << 7)) && --timeout);
        I2C_DR = Font5x7[font_index][col];
    }
    volatile uint32_t timeout = 5000;
    while (!(I2C_SR1 & (1 << 7)) && --timeout);
    I2C_DR = 0x00;
    I2C_stop();
}

void OLED_str(char *str) {
    while (*str) {
        OLED_char(*str++);
    }
}

void CAN_init() {
    CAN_MCR &= ~(1<<1);
    while(CAN_MSR & (1<<1));
    CAN_MCR |=  (1<<0);
    while(!(CAN_MSR & (1<<0)));

    // SJW = 3 (Bits 25:24 set to 0x3 for high clock drift tolerance)
    CAN_BTR = (3<<24) | (0x3<<20) | (0xA<<16) | (0<<0);

    CAN_FMR  |=  (1<<0);
    CAN_FA1R &= ~(1<<0);
    CAN_FS1R |=  (1<<0);
    CAN_FM1R &= ~(1<<0);
    CAN_F0R1  =   0x00;
    CAN_F0R2  =   0x00;
    CAN_FFA1R &= ~(1<<0);
    CAN_FA1R  |=  (1<<0);
    CAN_FMR   &= ~(1<<0);

    CAN_MCR &= ~(1<<0);
    while(CAN_MSR & (1<<0));
}

int32_t RX_temp = 0;
uint32_t RX_Pressure = 0;

void CAN_RX() {
    if(!(CAN_RF0R & 0x3)) return;

    uint32_t RI0R_read = CAN_RI0R;
    uint32_t check_rx_id = (RI0R_read >> 21) & 0x7FF;

    if(check_rx_id == 0x01) {
        RX_temp = (int32_t)CAN_RDL0R;
        RX_Pressure = (uint32_t)CAN_RDH0R;
    }
    CAN_RF0R |= (1 << 5);
}

void CAN_TX() {

    if (!(CAN_TSR & (1 << 26))) {

        volatile uint32_t delay = 500;
        while(delay--);
    }

    CAN_TI0R = (0x02 << 21);
    CAN_TDT0R = (0x01 << 0);


    if(!(GPIOA_IDR & (1<<0))) {
        CAN_TDL0R = 0x01;
    } else {
        CAN_TDL0R = 0x00;
    }
    CAN_TI0R |= (1 << 0);
}

void num_to_str(int32_t num, char *str) {
    uint8_t i = 0;
    int32_t temp_num = num;

    if(temp_num == 0) {
        str[i++] = '0';
    } else {
        while(temp_num > 0 && i < 8) {
            str[i++] = (temp_num % 10) + '0';
            temp_num /= 10;
        }
    }
    str[i] = '\0';

    uint8_t start = 0;
    uint8_t end = i - 1;
    while(start < end) {
        uint8_t temp_char = str[start];
        str[start] = str[end];
        str[end] = temp_char;
        start++;
        end--;
    }
}

int main(void) {
    GPIO_init();
    I2C_init();
    CAN_init();
    OLED_init();
    OLED_Clear();

    char str[12];

    while(1) {
        CAN_TX();
        CAN_RX();

        OLED_set_page_cursor(0, 2);
        OLED_str("TEMP:");
        num_to_str(RX_temp, str);
        OLED_str(str);
        OLED_str(" C ");

        OLED_set_page_cursor(4, 2);
        OLED_str("PRES:");
        num_to_str(RX_Pressure, str);
        OLED_str(str);
        OLED_str(" HPA ");

        for(volatile int i = 0; i < 200000; i++){}
    }
}
