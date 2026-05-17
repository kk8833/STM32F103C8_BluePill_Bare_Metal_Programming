#include <stdint.h>


#define APB2_CLK   (*(volatile uint32_t*)0x40021018)
#define PORTA_CRH  (*(volatile uint32_t*)0x40010804)

#define USART_SR   (*(volatile uint32_t*)0x40013800)
#define USART_DR   (*(volatile uint32_t*)0x40013804)
#define USART_BRR  (*(volatile uint32_t*)0x40013808)
#define USART_CR1  (*(volatile uint32_t*)0x4001380C)
#define USART_CR2  (*(volatile uint32_t*)0x40013810)

void GPIO_init()
{
    APB2_CLK |= (1<<2) | (1<<14) | (1<<0);  // GPIOA + USART1 + AFIO clocks

    PORTA_CRH &= ~(0xFF << 4);               // Clear PA9 and PA10 config
    PORTA_CRH |=  (0xB  << 4);              // PA9  TX → AF Push-Pull 50MHz
    PORTA_CRH |=  (0x4  << 8);              // PA10 RX → Input Floating
}

void USART_init()
{
    USART_BRR  =  (52 << 4) | 1;            // 8MHz → 9600 baud  (BRR = 0x0341)

    USART_CR2 &= ~((1<<12) | (1<<13));      // 1 stop bit (bits 13:12 = 00)

    USART_CR1 &= ~(1<<12);                  // 8 data bits (M bit = 0)
    USART_CR1 |=  (1<<13) | (1<<3) | (1<<2); // UE=1, TE=1, RE=1
}

void USART_SendChar(char c)
{
    while (!(USART_SR & (1<<7)));           // Wait until TXE=1 (DR is empty)
    USART_DR = c;
}

void USART_SendString(const char* str)
{
    while (*str)
    {
        USART_SendChar(*str++);
    }
}

char USART_ReceiveChar(void)
{
    while (!(USART_SR & (1<<5)));           // Wait until RXNE=1 (data ready)
    return (char)USART_DR;
}

int main(void)
{
    char RX_data = 0;                       // char not uint32_t, DR gives 1 byte

    GPIO_init();
    USART_init();

    USART_SendString("Hello __Krishnal__\r\n");

    RX_data = USART_ReceiveChar();

while (1)
{
    USART_SendString("Hello __Krishnal__\r\n");
    // add a small delay
    for(volatile int i = 0; i < 100000; i++);
}
}
