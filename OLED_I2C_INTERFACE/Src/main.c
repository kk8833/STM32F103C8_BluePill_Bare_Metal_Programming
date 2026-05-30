#include <stdint.h>

#define RCC_APB2ENR (*(volatile uint32_t*)0x40021018)
#define RCC_APB1ENR (*(volatile uint32_t*)0x4002101C)

#define PORTB_CRL  (*(volatile uint32_t*)0x40010C00)
#define PORTB_ODR  (*(volatile uint32_t*)0x40010C0C)

#define I2C_CR1   (*(volatile uint32_t*)0x40005400)
#define I2C_CR2   (*(volatile uint32_t*)0x40005404)
#define I2C_DR    (*(volatile uint32_t*)0x40005410)
#define I2C_SR1   (*(volatile uint32_t*)0x40005414)
#define I2C_SR2   (*(volatile uint32_t*)0x40005418)
#define I2C_CCR   (*(volatile uint32_t*)0x4000541C)
#define I2C_TRISE (*(volatile uint32_t*)0x40005420)

void GPIO_init() {
    RCC_APB2ENR |= (1 << 3) | (1 << 0);
    RCC_APB1ENR |= (1 << 21);

    PORTB_CRL &= ~(0xFF << 24);
    PORTB_CRL |=  (0xFF << 24);
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
    while (!(I2C_SR1 & (1 << 0)));
}

void I2C_stop() {
    while (!(I2C_SR1 & (1 << 7)));
    while (!(I2C_SR1 & (1 << 2)));
    I2C_CR1 |= (1 << 9);
}

void I2C_write_byte(uint8_t data) {
    while (!(I2C_SR1 & (1 << 7)));
    I2C_DR = data;
}

void I2C_send_addr(uint8_t address) {
    I2C_DR = (address << 1) | 0;
    while (!(I2C_SR1 & (1 << 1)));
    volatile uint32_t tmp = I2C_SR2;
    (void)tmp;
}

void OLED_cmd(uint8_t cmd) {
    I2C_start();
    I2C_send_addr(0x3C);
    I2C_write_byte(0x00);
    I2C_write_byte(cmd);
    I2C_stop();
}

void OLED_data(uint8_t data) {
    I2C_start();
    I2C_send_addr(0x3C);
    I2C_write_byte(0x40);
    I2C_stop();
}

void OLED_init() {
    OLED_cmd(0xAE);

    OLED_cmd(0xA1);
    OLED_cmd(0xC8);

    OLED_cmd(0xA8);
    OLED_cmd(0x3F);

    OLED_cmd(0xD3);
    OLED_cmd(0x00);

    OLED_cmd(0xDA);
    OLED_cmd(0x12);

    OLED_cmd(0x8D);
    OLED_cmd(0x14);


    OLED_cmd(0x20);
    OLED_cmd(0x02);

    OLED_cmd(0xAF);
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
        I2C_send_addr(0x3C);
        I2C_write_byte(0x40);
        for (uint8_t col = 0; col < 128; col++) {
            I2C_write_byte(0x00);
        }
        I2C_stop();
    }
}


const uint8_t test_word_5x7[4][5] = {
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 'T'
    {0x7F, 0x49, 0x49, 0x41, 0x41}, // 'E'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}  // 'T'
};

int main(void) {
    GPIO_init();
    I2C_init();
    OLED_init();


    OLED_Clear();


    OLED_set_page_cursor(0, 0);


    I2C_start();
    I2C_send_addr(0x3C);
    I2C_write_byte(0x40);

    for (uint8_t i = 0; i < 4; i++) {
        for (uint8_t j = 0; j < 5; j++) {
            while (!(I2C_SR1 & (1 << 7)));
            I2C_DR = test_word_5x7[i][j];
        }

        while (!(I2C_SR1 & (1 << 7)));
        I2C_DR = 0x00;
    }

    I2C_stop();

    while (1) {}
}
