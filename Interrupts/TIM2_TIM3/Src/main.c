
#include <stdint.h>

#define RCC_APB2ENR    (*(volatile uint32_t *)(0x40021018))
#define RCC_APB1ENR  (*(volatile uint32_t*)0x4002101C)
#define PORTA_CRL  (*(volatile uint32_t*)0x40010800)
#define GPIOA_IDR      (*(volatile uint32_t *)(0x40010808))
#define GPIOA_BSRR     (*(volatile uint32_t *)(0x40010810))


#define TIM2_CR1     (*(volatile uint32_t*)0x40000000)
#define TIM2_SR      (*(volatile uint32_t*)0x40000010)
#define TIM2_CNT     (*(volatile uint32_t*)0x40000024)
#define TIM2_PSC     (*(volatile uint32_t*)0x40000028)
#define TIM2_ARR     (*(volatile uint32_t*)0x4000002C)
#define TIM2_EGR     (*(volatile uint32_t*)0x40000014)

#define TIM2_DIER (*(volatile uint32_t*)0x4000000C)


#define TIM3_CR1     (*(volatile uint32_t*)0x40000400)
#define TIM3_SR      (*(volatile uint32_t*)0x40000410)
#define TIM3_CNT     (*(volatile uint32_t*)0x40000424)
#define TIM3_PSC     (*(volatile uint32_t*)0x40000428)
#define TIM3_ARR     (*(volatile uint32_t*)0x4000042C)
#define TIM3_EGR     (*(volatile uint32_t*)0x40000414)
#define TIM3_DIER (*(volatile uint32_t*)0x4000040C)

#define IPR7  (*(volatile uint32_t*)0xE000E41C)
#define ISER0 (*(volatile uint32_t*)0xE000E100)
#define ICER0 (*(volatile uint32_t*)0xE000E080)
#define ISPR0 (*(volatile uint32_t*)0xE000E200)
#define ICPR0 (*(volatile uint32_t*)0xE000E280)
#define IABR0 (*(volatile uint32_t*)0xE000E300)


void GPIO_init()
{
	RCC_APB2ENR|=(1<<2);
	RCC_APB1ENR|=(1<<0)|(1<<1);
	PORTA_CRL &= ~(0xFF<<0);
	PORTA_CRL |=(0x22<<0);

}
void  TIM2_init()
{
	TIM2_PSC=7999;
	TIM2_ARR=999;
	TIM2_EGR|=(1<<0);
	TIM2_DIER|=(1<<0);
	TIM2_CR1|=(1<<0);
}
void TIM3_init()
{
	TIM3_PSC=7999;
	TIM3_ARR=499;
	TIM3_EGR|=(1<<0);
	TIM3_DIER|=(1<<0);
	TIM3_CR1|=(1<<0);

}

void Interrupt_init()
{
IPR7 &= ~(0XFFFF<<0);
IPR7 |=(1<<4)|(2<<12);
ISER0 |=(1<<28)|(1<<29);


}
void TIM2_IRQHandler()
{

	if (TIM2_SR &(1<<0))
	{
		TIM2_SR &= ~(1<<0); //clear the flag

		if(GPIOA_IDR &(1<<0))
				{
				GPIOA_BSRR=(1<<16);
				}
		else
		{
		GPIOA_BSRR=(1<<0);
		}


	}
}
void TIM3_IRQHandler()
{
	if(TIM3_SR &(1<<0))
	{
		TIM3_SR &= ~(1<<0);  //clear the flag
		if(GPIOA_IDR & (1<<1))
		{
			GPIOA_BSRR =(1<<17);
		}
		else
		{
			GPIOA_BSRR =(1<<1);
		}



	}
}
int main(void)
{GPIO_init();
	TIM2_init();
	TIM3_init();
	Interrupt_init();

    while(1)
    {
    	;
    }
}
