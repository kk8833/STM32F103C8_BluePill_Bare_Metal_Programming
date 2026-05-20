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


#define TIM3_CR1     (*(volatile uint32_t*)0x40000400)
#define TIM3_SR      (*(volatile uint32_t*)0x40000410)
#define TIM3_CCMR1   (*(volatile uint32_t*)0x40000418)
#define TIM3_CCER    (*(volatile uint32_t*)0x40000420)
#define TIM3_CNT     (*(volatile uint32_t*)0x40000424)
#define TIM3_PSC     (*(volatile uint32_t*)0x40000428)
#define TIM3_ARR     (*(volatile uint32_t*)0x4000042C)
#define TIM3_CCR1    (*(volatile uint32_t*)0x40000434)
#define TIM3_EGR     (*(volatile uint32_t*)0x40000414)

#define USART_SR   (*(volatile uint32_t*)0x40013800)
#define USART_DR   (*(volatile uint32_t*)0x40013804)
#define USART_BRR  (*(volatile uint32_t*)0x40013808)
#define USART_CR1  (*(volatile uint32_t*)0x4001380C)
#define USART_CR2  (*(volatile uint32_t*)0x40013810)


void GPIO_init()
{  //Clock Enables for the peripherals
	RCC_APB2ENR |= (1<<2)|(1<<3)|(1<<14)|(1<<0);
	RCC_APB1ENR |= (1<<0)|(1<<1);

	//CONFIGURATION  of the GPIO pins
	// SERVO PIN AND ECHO PIN
	PORTA_CRL &= ~((0XF<<0)|(0XF<<24));
	PORTA_CRL |= (0XB<<0)|(0X4<<24);
	// USART1 PA9-TX PA10-RX CONFIG
	PORTA_CRH &= ~((0XFF<<4));
	PORTA_CRH |= (0xB<<4)|(0X4<<8);
	//Buzzer-PB1 and Trig-PB0  PIN config
	GPIOB_CRL &= ~(0XFF<<0);
	GPIOB_CRL |=(0X33<<0);

}
void TIM2_PWM_init()
{   //clock is  running at 8mhz so to get 1us
	//tick per clock 8mhz/psc+1  =8mhz/7+1 =1mhz clock tick_per_sec=1/1mhz=1us
	//to get 50hz freq for pwm 1/50 =20ms which is 20,000 us
	// set ARR to 19999 to get 20ms then
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
	TIM3_CNT=0; //reset to 0 first.

	while (TIM3_CNT<us);

}
void TIM3_FOR_SENSOR_init()
{
	TIM3_PSC =7;
	TIM3_ARR=65535;
	TIM3_EGR |=(1<<0);
	TIM3_CR1 |=(1<<0);


}
void USART1_init()
{
	USART_BRR =(52<<4)|1;  // 8MHZ ,9600  8/(16 X 9600)=52.083 ,0.083X16=1.328 ~=1
	USART_CR1|=(1<<2)|(1<<3)|(1<<13);
	USART_CR1 &= ~(1<<12);
	USART_CR2 &= ~((1<<12)|(1<<13));

}

void USART_TX(uint8_t word)
{
	while(!(USART_SR & (1<<7)));
	USART_DR= word;

}
void USART_string(char  *str)
{
	while(*str)
	{
		USART_TX(*str++);

	}
}

void Distance_TX(uint16_t num)
{
	char buffer[6];
	uint8_t i=0;
	if (num==0)
	{
		USART_TX('0');
		return;
	}

	while(num>0)
	{
		buffer[i++]= (num%10)+'0';
		num=num/10;
	}
	while (i>0)
	{
		USART_TX(buffer[--i]);
	}
}


int main(void) {

GPIO_init();
TIM2_PWM_init();
TIM3_FOR_SENSOR_init();
USART1_init();

 GPIOB_BSRR =(1<<1); //buzzer is active low so keep off at start.


    while(1) {


    	for(uint16_t Capture_val=1000; Capture_val<2000; Capture_val+=10)
    	{
    		    TIM2_CCR1=Capture_val;
    		    delay_us(15000);

    		    GPIOB_BSRR = (1<<0);
    		        	delay_us(10);
    		        	GPIOB_BSRR = (1<<(0+16));


    		        	while((GPIOA_IDR & (1<<6))==0)
    		        	  {
    		        		if(TIM3_CNT > 30000) break;
    		        	  }
    		        	  TIM3_CNT=0;
    		        	while((GPIOA_IDR &(1<<6))!=0)
    		        	{
    		        		if(TIM3_CNT > 30000) break;
    		        	}
    		        	uint16_t Echo_pulse= TIM3_CNT;
    		        	uint16_t Distance = Echo_pulse /58;
    		            uint16_t Angle = ((Capture_val -1000) *180)/1000;
    		        	Distance_TX(Angle);
						USART_TX(',');
						Distance_TX(Distance);
						USART_string("\r\n");

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


    	    		        	while((GPIOA_IDR & (1<<6))==0)
    	    		        	{
    	    		        		if(TIM3_CNT > 30000) break;
    	    		        	}
    	    		        	  TIM3_CNT=0;
    	    		        	while((GPIOA_IDR &(1<<6))!=0)
    	    		        	{
    	    		        		if(TIM3_CNT > 30000) break;
    	    		        	}
    	    		        	uint16_t Echo_pulse= TIM3_CNT;
    	    		        	uint16_t Distance = Echo_pulse /58;
                                uint16_t Angle = ((Capture_val -1000) *180)/1000;
    	    		        	Distance_TX(Angle);
    	    		        	USART_TX(',');
    	    		        	Distance_TX(Distance);
    	    		        	USART_string("\r\n");

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
