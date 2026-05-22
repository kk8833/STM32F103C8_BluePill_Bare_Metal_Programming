
#include <stdint.h>
#define RCC_APB2ENR (*(volatile uint32_t*)0x40021018)
#define RCC_APB1ENR (*(volatile uint32_t*)0x4002101C)
#define GPIOA_CRH (*(volatile uint32_t*)0x40010804)


#define CAN_MCR (*(volatile uint32_t*)0x40006400)
#define CAN_MSR (*(volatile uint32_t*)0x40006404)
#define CAN_BTR (*(volatile uint32_t*)0x4000641C)

#define CAN_FMR (*(volatile uint32_t*)0x40006600)
#define CAN_FM1R (*(volatile uint32_t*)0x40006604)
#define CAN_FS1R (*(volatile uint32_t*)0x4000660C)
#define CAN_FFA1R (*(volatile uint32_t*)0x40006614)
#define CAN_FA1R (*(volatile uint32_t*)0x4000661C)
#define CAN_F0R1 (*(volatile uint32_t*)0x40006640)
#define CAN_F0R2 (*(volatile uint32_t*)0x40006644)

#define CAN_TSR (*(volatile uint32_t*)0x40006408)
#define CAN_TI0R (*(volatile uint32_t*)0x40006580)
#define  CAN_TDT0R (*(volatile uint32_t*)0x40006584)
#define CAN_TDL0R (*(volatile uint32_t*)0x40006588)
#define CAN_TDH0R (*(volatile uint32_t*)0x4000658C)

#define CAN_RF0R (*(volatile uint32_t*)0x4000640C)
#define CAN_RI0R (*(volatile uint32_t*)0x400065B0)
#define CAN_RDT0R (*(volatile uint32_t*)0x400065B4)
#define CAN_RDL0R (*(volatile uint32_t*)0x400065B8)
#define CAN_RDH0R (*(volatile uint32_t*)0x400065BC)

void GPIO_init()
{
	RCC_APB2ENR |=(1<<2);
	RCC_APB1ENR|=(1<<25);
	GPIOA_CRH &= ~(0XFF<<12);
	GPIOA_CRH |=(0xB<<16)|(0X4<<12);
}
void CAN_init()
{
	CAN_MCR = (1<<0) | (1<<6);
	while(!(CAN_MSR & (1<<0)));
	while(CAN_MSR & (1<<1));
	CAN_BTR &= ~((0x3 << 30) | (0x7 << 20) | (0xF << 16) | (0x3FF << 0));
	 CAN_BTR =(0x0<<0)|(0XA<<16)|(0X3<<20)|(1<<30);

	CAN_FMR |=(1<<0);
	CAN_FA1R  &= ~ (1<<0);
	CAN_FM1R &=~(1<<0);
	CAN_FS1R |=(1<<0);
	CAN_FFA1R &=~(1<<0);
	CAN_F0R1 =(0X123<<21);
	CAN_F0R2 =(0x7FF<<21);
	CAN_FA1R |=(1<<0);
	CAN_FMR  &= ~(1<<0);
	CAN_MCR &= ~(1<<0);
	while(CAN_MSR & (1<<0));




}
int main(void)
{ GPIO_init();
CAN_init();
    while(1)
    {
    	while(!(CAN_TSR &(1<<26)));
    	CAN_TDL0R=(0x4B<<0)|(0X52<<8)|(0X49<<16)|(0X53<<24);
    	CAN_TDH0R=(0X48<<0)|(0X4E<<8)|(0X41<<16)|(0X4C<<24);
    	CAN_TDT0R =(0X8<<0);
    	CAN_TI0R =(0X123<<21);
    	CAN_TI0R |=(1<<0);
    	while(!(CAN_TSR & (1<<1)));

        while((CAN_RF0R & ((1<<1)|(1<<0)))<1);
      uint32_t RI0R_read= CAN_RI0R;
      uint32_t check_rx_id= (RI0R_read>>21) &0x7FF;
      uint32_t RX_Low_byte=CAN_RDL0R;
      uint32_t RX_High_byte=CAN_RDH0R;
      CAN_RF0R |=(1<<5);

    }
}
