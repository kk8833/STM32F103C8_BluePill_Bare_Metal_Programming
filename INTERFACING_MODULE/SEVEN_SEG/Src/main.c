
#include<stdint.h>
#define Port_B_Clock_Enable (*(volatile  uint32_t*)0x40021018)
#define Configure_Port (*(volatile uint32_t*)0x40010C00)
#define  GPIOB_ODR (*(volatile uint32_t*)0x40010C0C)

void delay(uint32_t count)
{
	while(count--);
}

int main(void)
{
	uint8_t seg_code[] = {
			0x3F,
	    0x06,
	    0x5B,
	    0x4F,
	    0x66,
	    0x6D,
	    0x7D,
	    0x07,
	    0x7F,
	    0x6F  };
	Port_B_Clock_Enable|=(1<<3);
	Configure_Port &= ~(0xFFFFFFF);
	Configure_Port |=(0x2<<0)|(0x2<<4)|(0x2<<8)|(0x2<<12)|(0x2<<16)|(0x2<<20)|(0x2<<24);


	for(;;)
	{
		for (uint8_t i=0; i<10; i++)
		{

			GPIOB_ODR= seg_code[i];
			delay(500000);
		}
	}
}
