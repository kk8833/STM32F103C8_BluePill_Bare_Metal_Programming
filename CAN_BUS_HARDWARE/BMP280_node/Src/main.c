#include <stdint.h>

/* ---- RCC ---- */
#define RCC_APB2ENR (*(volatile uint32_t*)0x40021018)
#define RCC_APB1ENR (*(volatile uint32_t*)0x4002101C)

/* ---- GPIO PORTB (I2C & CAN) ---- */
#define GPIOB_CRL   (*(volatile uint32_t*)0x40010C00)
#define GPIOB_CRH   (*(volatile uint32_t*)0x40010C04)

/* ---- GPIO PORTA (Actuator Pin) ---- */
#define GPIOA_CRL   (*(volatile uint32_t*)0x40010800)
#define GPIOA_ODR   (*(volatile uint32_t*)0x4001080C)

/* ---- PIN remap for CAN ---- */
#define AFIO_MAPR   (*(volatile uint32_t*)0x40010004)

/* ---- I2C1 ---- */
#define I2C_CR1   (*(volatile uint32_t*)0x40005400)
#define I2C_CR2   (*(volatile uint32_t*)0x40005404)
#define I2C_DR    (*(volatile uint32_t*)0x40005410)
#define I2C_SR1   (*(volatile uint32_t*)0x40005414)
#define I2C_SR2   (*(volatile uint32_t*)0x40005418)
#define I2C_CCR   (*(volatile uint32_t*)0x4000541C)
#define I2C_TRISE (*(volatile uint32_t*)0x40005420)

/* ---- CAN registers ---- */
#define CAN_MCR     (*(volatile uint32_t*)0x40006400)
#define CAN_MSR     (*(volatile uint32_t*)0x40006404)
#define CAN_BTR     (*(volatile uint32_t*)0x4000641C)
#define CAN_TSR     (*(volatile uint32_t*)0x40006408)
#define CAN_RF0R    (*(volatile uint32_t*)0x4000640C)

#define CAN_FMR     (*(volatile uint32_t*)0x40006600)
#define CAN_FM1R    (*(volatile uint32_t*)0x40006604)
#define CAN_FS1R    (*(volatile uint32_t*)0x4000660C)
#define CAN_FFA1R   (*(volatile uint32_t*)0x40006614)
#define CAN_FA1R    (*(volatile uint32_t*)0x4000661C)
#define CAN_F0R1    (*(volatile uint32_t*)0x40006640)
#define CAN_F0R2    (*(volatile uint32_t*)0x40006644)

/* ---- Mailboxes ---- */
#define CAN_TI0R    (*(volatile uint32_t*)0x40006580)
#define CAN_TDT0R   (*(volatile uint32_t*)0x40006584)
#define CAN_TDL0R   (*(volatile uint32_t*)0x40006588)
#define CAN_TDH0R   (*(volatile uint32_t*)0x4000658C)

#define CAN_RI0R    (*(volatile uint32_t*)0x400065B0)
#define CAN_RDL0R   (*(volatile uint32_t*)0x400065B8)

#define BMP280_ADDR 0x76

/* =========================================================
   I2C — EXACT copy from working UART test code
   ========================================================= */
void I2C_init(void) {
    I2C_CR1 &= ~(1 << 0);
    I2C_CR2  = 0x08;
    I2C_CCR  = 0x28;
    I2C_TRISE = 0x09;
    I2C_CR1 |= (1 << 0);
}

static uint8_t i2c_start(void) {
    I2C_CR1 |= (1 << 8);
    uint32_t t = 50000;
    while (!(I2C_SR1 & 1)) if (!--t) return 0;
    return 1;
}

static void i2c_stop(void) { I2C_CR1 |= (1 << 9); }

static uint8_t i2c_addr(uint8_t addr, uint8_t rw) {
    I2C_DR = (addr << 1) | rw;
    uint32_t t = 50000;
    while (!(I2C_SR1 & (1 << 1))) {
        if (I2C_SR1 & (1 << 10)) { I2C_SR1 &= ~(1 << 10); i2c_stop(); return 0; }
        if (!--t) { i2c_stop(); return 0; }
    }
    volatile uint32_t tmp = I2C_SR2; (void)tmp;
    return 1;
}

static uint8_t i2c_write_byte(uint8_t b) {
    uint32_t t = 50000;
    while (!(I2C_SR1 & (1 << 7))) if (!--t) return 0;
    I2C_DR = b;
    t = 50000;
    while (!(I2C_SR1 & (1 << 2))) if (!--t) return 0;
    return 1;
}

static void i2c_read_bytes(uint8_t *buf, uint8_t n) {
    if (n == 1) {
        I2C_CR1 &= ~(1 << 10);
        volatile uint32_t tmp = I2C_SR2; (void)tmp;
        i2c_stop();
        uint32_t t = 50000;
        while (!(I2C_SR1 & (1 << 6))) if (!--t) break;
        buf[0] = I2C_DR;
        return;
    }
    I2C_CR1 |= (1 << 10);
    volatile uint32_t tmp = I2C_SR2; (void)tmp;
    for (uint8_t i = 0; i < n; i++) {
        if (i == n - 1) { I2C_CR1 &= ~(1 << 10); i2c_stop(); }
        uint32_t t = 50000;
        while (!(I2C_SR1 & (1 << 6))) if (!--t) break;
        buf[i] = I2C_DR;
    }
}

void bmp_read_regs(uint8_t reg, uint8_t *buf, uint8_t n) {
    if (!i2c_start()) return;
    if (!i2c_addr(BMP280_ADDR, 0)) return;
    i2c_write_byte(reg);
    if (!i2c_start()) return;
    if (!i2c_addr(BMP280_ADDR, 1)) return;
    i2c_read_bytes(buf, n);
}

void bmp_write_reg(uint8_t reg, uint8_t val) {
    if (!i2c_start()) return;
    if (!i2c_addr(BMP280_ADDR, 0)) return;
    i2c_write_byte(reg);
    i2c_write_byte(val);
    i2c_stop();
}

/* =========================================================
   BMP280 Calibration
   ========================================================= */
uint16_t dig_T1; int16_t dig_T2, dig_T3;
uint16_t dig_P1; int16_t dig_P2, dig_P3, dig_P4,
                          dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
int32_t t_fine;

void bmp_read_calib(void) {
    uint8_t c[24];
    bmp_read_regs(0x88, c, 24);
    dig_T1 = (c[1]<<8)|c[0]; dig_T2 = (c[3]<<8)|c[2]; dig_T3 = (c[5]<<8)|c[4];
    dig_P1 = (c[7]<<8)|c[6]; dig_P2 = (c[9]<<8)|c[8]; dig_P3 = (c[11]<<8)|c[10];
    dig_P4 = (c[13]<<8)|c[12]; dig_P5 = (c[15]<<8)|c[14];
    dig_P6 = (c[17]<<8)|c[16]; dig_P7 = (c[19]<<8)|c[18];
    dig_P8 = (c[21]<<8)|c[20]; dig_P9 = (c[23]<<8)|c[22];
}

int32_t bmp_compensate_temp(int32_t adc_T) {
    int32_t var1 = ((((adc_T>>3)-((int32_t)dig_T1<<1)))*((int32_t)dig_T2))>>11;
    int32_t var2 = (((((adc_T>>4)-(int32_t)dig_T1)*((adc_T>>4)-(int32_t)dig_T1))>>12)*((int32_t)dig_T3))>>14;
    t_fine = var1 + var2;
    return (t_fine*5+128)>>8;
}

uint32_t bmp_compensate_press(int32_t adc_P) {
    int64_t var1 = ((int64_t)t_fine) - 128000;
    int64_t var2 = var1*var1*(int64_t)dig_P6;
    var2 += ((var1*(int64_t)dig_P5)<<17);
    var2 += (((int64_t)dig_P4)<<35);
    var1 = ((var1*var1*(int64_t)dig_P3)>>8)+((var1*(int64_t)dig_P2)<<12);
    var1 = (((int64_t)1<<47)+var1)*((int64_t)dig_P1)>>33;
    if (var1 == 0) return 0;
    int64_t p = 1048576 - adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)dig_P9)*(p>>13)*(p>>13))>>25;
    var2 = (((int64_t)dig_P8)*p)>>19;
    p = ((p+var1+var2)>>8)+(((int64_t)dig_P7)<<4);
    return (uint32_t)(p>>8);
}


void GPIO_init(void) {
    RCC_APB2ENR |= (1 << 0) | (1 << 2) | (1 << 3);
    RCC_APB1ENR |= (1 << 21) | (1 << 25);

    /* PA0 as Output Push-Pull */
    GPIOA_CRL &= ~(0xF << 0);
    GPIOA_CRL |=  (0x2 << 0);
    GPIOA_ODR &= ~(1 << 0);

    /* PB6 & PB7 as AF Open-Drain (I2C1) */
    GPIOB_CRL &= ~(0xFF << 24);
    GPIOB_CRL |=  (0xFF << 24);

    /* PB8 (CAN_RX) Input, PB9 (CAN_TX) AF Push-Pull */
    GPIOB_CRH &= ~(0xFF << 0);
    GPIOB_CRH |=  (0xB4 << 0);

    /* Remap CAN to PB8/PB9 */
    AFIO_MAPR &= ~(0x3 << 13);
    AFIO_MAPR |=  (0x2 << 13);
}


void CAN_init(void) {
    CAN_MCR &= ~(1 << 1);
    while (CAN_MSR & (1 << 1));
    CAN_MCR |=  (1 << 0);
    while (!(CAN_MSR & (1 << 0)));

    CAN_BTR = (3 << 24) | (0x3 << 20) | (0xA << 16) | (0 << 0);

    CAN_FMR  |=  (1 << 0);
    CAN_FA1R &= ~(1 << 0);
    CAN_FS1R |=  (1 << 0);
    CAN_FM1R &= ~(1 << 0);
    CAN_F0R1  =  0x00;
    CAN_F0R2  =  0x00;
    CAN_FFA1R &= ~(1 << 0);
    CAN_FA1R  |=  (1 << 0);
    CAN_FMR   &= ~(1 << 0);

    CAN_MCR &= ~(1 << 0);
    while (CAN_MSR & (1 << 0));
}

int32_t  actual_temp     = 0;
uint32_t actual_pressure = 0;

void CAN_TX(void) {
    if (!(CAN_TSR & (1 << 26))) {
        CAN_TSR |= (1 << 23);
        volatile uint32_t d = 500; while (d--);
    }
    CAN_TI0R  = (0x01 << 21);
    CAN_TDT0R = 8;
    CAN_TDL0R = (uint32_t)actual_temp;
    CAN_TDH0R = (uint32_t)actual_pressure;
    CAN_TI0R |= (1 << 0);
}

void CAN_RX(void) {
    if (!(CAN_RF0R & 0x3)) return;
    uint32_t id = (CAN_RI0R >> 21) & 0x7FF;
    if (id == 0x02) {
        uint8_t b = CAN_RDL0R & 0xFF;
        if (b == 1) GPIOA_ODR |=  (1 << 0);
        else        GPIOA_ODR &= ~(1 << 0);
    }
    CAN_RF0R |= (1 << 5);
}


void delay_ms(uint32_t ms) {
    for (; ms; ms--)
        for (volatile uint32_t i = 0; i < 800; i++);
}

int main(void) {
    GPIO_init();
    I2C_init();
    CAN_init();

    delay_ms(100);

    bmp_read_calib();

    bmp_write_reg(0xF4, 0x93);
    bmp_write_reg(0xF5, 0xA0);

    delay_ms(100);

    while (1)
    {
        uint8_t raw[6];
        bmp_read_regs(0xF7, raw, 6);

        int32_t adc_P = ((int32_t)raw[0]<<12)|((int32_t)raw[1]<<4)|(raw[2]>>4);
        int32_t adc_T = ((int32_t)raw[3]<<12)|((int32_t)raw[4]<<4)|(raw[5]>>4);

        actual_temp     = bmp_compensate_temp(adc_T) / 100;
        actual_pressure = bmp_compensate_press(adc_P) / 100;

        CAN_TX();
        CAN_RX();

        delay_ms(500);
    }
}
