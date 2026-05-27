
#include <stdint.h>
#define RCC_APB2ENR    (*(volatile uint32_t *)(0x40021018))
#define RCC_APB1ENR  (*(volatile uint32_t*)0x4002101C)

#define PORTA_CRH  (*(volatile uint32_t*)0x40010804)
#define PORTA_CRL  (*(volatile uint32_t*)0x40010800)
#define GPIOA_IDR      (*(volatile uint32_t *)(0x40010808))
#define GPIOA_BSRR     (*(volatile uint32_t *)(0x40010810))

#define GPIOB_CRH      (*(volatile uint32_t *)(0x40010C04))
#define GPIOB_CRL   (*(volatile uint32_t*)0x40010C00)
#define GPIOB_IDR   (*(volatile uint32_t*)0x40010C08)
#define GPIOB_BSRR     (*(volatile uint32_t *)(0x40010C10))


#define TIM2_CR1     (*(volatile uint32_t*)0x40000000)
#define TIM2_SR      (*(volatile uint32_t*)0x40000010)
#define TIM2_CCMR1   (*(volatile uint32_t*)0x40000018)
#define TIM2_CCER    (*(volatile uint32_t*)0x40000020)
#define TIM2_CNT     (*(volatile uint32_t*)0x40000024)
#define TIM2_PSC     (*(volatile uint32_t*)0x40000028)
#define TIM2_ARR     (*(volatile uint32_t*)0x4000002C)
#define TIM2_CCR1    (*(volatile uint32_t*)0x40000034)
#define TIM2_EGR     (*(volatile uint32_t*)0x40000014)
#define TIM2_DIER     (*(volatile uint32_t*)0x4000000C)


#define TIM3_CR1     (*(volatile uint32_t*)0x40000400)
#define TIM3_SR      (*(volatile uint32_t*)0x40000410)
#define TIM3_CCMR1   (*(volatile uint32_t*)0x40000418)
#define TIM3_CCER    (*(volatile uint32_t*)0x40000420)
#define TIM3_CNT     (*(volatile uint32_t*)0x40000424)
#define TIM3_PSC     (*(volatile uint32_t*)0x40000428)
#define TIM3_ARR     (*(volatile uint32_t*)0x4000042C)
#define TIM3_CCR1    (*(volatile uint32_t*)0x40000434)
#define TIM3_EGR     (*(volatile uint32_t*)0x40000414)
#define TIM3_DIER     (*(volatile uint32_t*)0x4000040C)


#define USART_SR   (*(volatile uint32_t*)0x40013800)
#define USART_DR   (*(volatile uint32_t*)0x40013804)
#define USART_BRR  (*(volatile uint32_t*)0x40013808)
#define USART_CR1  (*(volatile uint32_t*)0x4001380C)
#define USART_CR2  (*(volatile uint32_t*)0x40013810)

#define IPR7  (*(volatile uint32_t*)0xE000E41C)
#define IPR9 (*(volatile uint32_t*)0xE000E424)
#define ISER0 (*(volatile uint32_t*)0xE000E100)
#define ICER0 (*(volatile uint32_t*)0xE000E080)
#define ISPR0 (*(volatile uint32_t*)0xE000E200)
#define ICPR0 (*(volatile uint32_t*)0xE000E280)
#define IABR0 (*(volatile uint32_t*)0xE000E300)

volatile uint16_t Echo_Pulse=0;
volatile uint16_t Distance =0;
volatile uint8_t  edge_state  = 0;
volatile uint16_t rise_time   = 0;
volatile uint8_t  data_ready  = 0;

void GPIO_init()
{
	RCC_APB2ENR |= (1<<2)|(1<<3)|(1<<14)|(1<<0);
	RCC_APB1ENR |= (1<<0)|(1<<1);
	PORTA_CRL &= ~((0XF<<0)|(0XF<<24));
	PORTA_CRL |= (0XB<<0)|(0X4<<24);
	PORTA_CRH &= ~((0XFF<<4));
	PORTA_CRH |= (0xB<<4)|(0X4<<8);
	GPIOB_CRL &= ~(0XFF<<0);
	GPIOB_CRL |=(0X33<<0);

}

void TIM2_init()
{
	TIM2_PSC=7;
	TIM2_ARR=19999;
	TIM2_EGR|=(1<<0);
	TIM2_CCMR1 &= ~(0XFF<<0);
	TIM2_CCMR1 |=(0X68<<0);
	TIM2_CCER|=(1<<0);
	TIM2_CCR1=1000;
	TIM2_CR1|=(1<<0);
}

void delay_us(uint16_t us)
{
	TIM3_CNT=0;
	while(TIM3_CNT<us);

}
void TIM3_init()
{   TIM3_PSC =7;
    TIM3_ARR=65535;
    TIM3_EGR |=(1<<0);
    TIM3_CCMR1 &= ~(0xFF<<0);
    TIM3_CCMR1 |=  (0x01<<0);
    TIM3_CCER  &= ~(1<<1);
	TIM3_CCER  |=  (1<<0);
	TIM3_DIER &= ~((0X3<<0)|(1<<6));
	TIM3_DIER |=(1<<1);
	TIM3_CCER|=(1<<0);
	TIM3_CCER&= ~(1<<1);
	TIM3_CR1|=(1<<0);

}

void USART_init()
{
	USART_BRR =(52<<4)|1;
	USART_CR1|=(1<<2)|(1<<3)|(1<<13);
	USART_CR1 &= ~(1<<12);
	USART_CR2 &= ~((1<<12)|(1<<13));


}
void USART_TX(uint8_t word)
{
	while(!(USART_SR & (1<<7)));
	USART_DR= word;

}
void Distance_TX (uint16_t Dist)
{
  char  char_buff[6];
	uint8_t i=0;
	if (Dist==0)
	{

		USART_TX('0');
		return;
	}
	while(Dist>0)
	{    char_buff[i++]= Dist%10+'0';
				Dist=Dist/10;
	}
	while (i>0)
		{
			USART_TX(char_buff[--i]);
		}
}

void ISR_init()
{
	IPR7|=(0x0<<12);
   ISER0|=(1<<29);


}
void TIM3_IRQHandler()
{
	if(TIM3_SR & (1<<1))
	{    TIM3_SR &= ~(1<<1);
	if(edge_state == 0)
	        {

	            rise_time  = TIM3_CCR1;
	            TIM3_CCER |= (1<<1);
	            edge_state = 1;
	        }
	        else
	        {

	            Echo_Pulse = TIM3_CCR1 - rise_time;
	            Distance   = Echo_Pulse / 58;
	            TIM3_CCER &= ~(1<<1);
	            edge_state  = 0;
	            data_ready  = 1;
	        }
	}

}

void USART_STR(char *str)
{
	while(*str)
	{
		USART_TX(*str++);
	}
}

int main(void)
{   GPIO_init();
USART_init();
TIM2_init();
TIM3_init();
ISR_init();

GPIOB_BSRR =(1<<1);

	 while(1) {


	    	for(uint16_t Capture_val=1000; Capture_val<2000; Capture_val+=10)
	    	{
	    		    TIM2_CCR1=Capture_val;
	    		    delay_us(15000);

	    		    GPIOB_BSRR = (1<<0);
					delay_us(10);
					GPIOB_BSRR = (1<<(0+16));
					 data_ready = 0;
				uint32_t timeout = 0;
					while(data_ready == 0)
					            {
					                timeout++;
					                if(timeout > 50000) break;
					            }





	    		    uint16_t Angle = ((Capture_val -1000) *180)/1000;
	    		        	Distance_TX(Angle);
							USART_TX(',');
							Distance_TX(Distance);
							USART_STR("\r\n");

	    		            if(Distance>2 && Distance<=60)
	    		            {
	    		            	GPIOB_BSRR =(1<<(1+16));

	    		            }
	    		            else
	    		            {
	    		            	GPIOB_BSRR=(1<<1);
	    		            }

	    	}


	    	for(int16_t Capture_val=2000; Capture_val>=1000; Capture_val-=10)
	    	    	{
	    	    		    TIM2_CCR1=Capture_val;
	    	    		    delay_us(15000);

	    	    		    GPIOB_BSRR = (1<<0);
							delay_us(10);
							GPIOB_BSRR = (1<<(0+16));
							 data_ready = 0;
							            uint32_t timeout = 0;
							            while(data_ready == 0)
							            {
							                timeout++;
							                if(timeout > 50000) break;
							            }





	                                uint16_t Angle = ((Capture_val -1000) *180)/1000;
	    	    		        	Distance_TX(Angle);
	    	    		        	USART_TX(',');
	    	    		        	Distance_TX(Distance);
	    	    		        	USART_STR("\r\n");

	    	    		            if(Distance>2 && Distance<=60)
	    	    		            {
	    	    		            	GPIOB_BSRR =(1<<(1+16));

	    	    		            }
	    	    		            else
	    	    		            {
	    	    		            	GPIOB_BSRR=(1<<1);
	    	    		            }

	    	    	}







	    }
}
