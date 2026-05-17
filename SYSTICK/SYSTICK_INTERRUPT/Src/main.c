
#include <stdint.h>
#define SYS_CONTROL_REG (*(volatile uint32_t*)0xE000E010)
#define LOAD_REG (*(volatile uint32_t*)0xE000E014)
#define VAL (*(volatile uint32_t*)0xE000E018)

#define Clock_Enable (*(volatile uint32_t*)0x40021018)
#define Port_A_configure (*(volatile uint32_t*)0x40010800)
#define Bit_Set (*(volatile uint32_t*)0x40010810)

volatile uint32_t count=0;
volatile uint32_t led_state=0;
void SYS_init()
{

	LOAD_REG=7999;
	VAL=0;
	SYS_CONTROL_REG=(1<<0)|(1<<2)|(1<<1);

}

void SysTick_Handler(void)
{
   count++;
}
int main(void)
{
   SYS_init();
	Clock_Enable|=(1<<2);
	Port_A_configure &= ~(0xF<<0);
	Port_A_configure|=(0x2<<0);


	for(;;)
	{
		if(count >= 200)
		{          count = 0;

		            if(led_state == 0)
		            {
		                Bit_Set   = (1<<0);
		                led_state = 1;
		            }
		            else
		            {
		                Bit_Set   = (1<<16);
		                led_state = 0;
		            }
		        }

	}

	}

