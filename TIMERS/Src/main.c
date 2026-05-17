#include <stdint.h>

#define RCC_APB2ENR  (*(volatile uint32_t*)0x40021018)
#define RCC_APB1ENR  (*(volatile uint32_t*)0x4002101C)

#define GPIOA_CRL    (*(volatile uint32_t*)0x40010800)


#define TIM2_CR1     (*(volatile uint32_t*)0x40000000)
#define TIM2_CCMR1   (*(volatile uint32_t*)0x40000018)
#define TIM2_CCER    (*(volatile uint32_t*)0x40000020)
#define TIM2_PSC     (*(volatile uint32_t*)0x40000028)
#define TIM2_ARR     (*(volatile uint32_t*)0x4000002C)
#define TIM2_CCR1    (*(volatile uint32_t*)0x40000034)

void delay_ms(volatile uint32_t ms)
{

    while (ms--)
    {
        volatile uint32_t i = 800;
        while (i--);
    }
}

int main(void)
{

    RCC_APB2ENR |= (1 << 2);
    RCC_APB1ENR |= (1 << 0);


    GPIOA_CRL &= ~(0xF << 0);
    GPIOA_CRL |=  (0xB << 0);


    TIM2_PSC = 7;


    TIM2_ARR = 19999;


    TIM2_CCMR1 &= ~(0xFF << 0);
    TIM2_CCMR1 |=  (0x68 << 0);


    TIM2_CCER |= (1 << 0);


    TIM2_CCR1 = 1500;


    TIM2_CR1 |= (1 << 0);

    while (1)
    {
        TIM2_CCR1 = 600;
        delay_ms(2000);

        TIM2_CCR1 = 2400;
        delay_ms(2000);
    }
}
