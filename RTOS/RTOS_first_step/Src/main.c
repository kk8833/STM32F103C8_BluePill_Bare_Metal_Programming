#include "FreeRTOS.h"
#include "task.h"

#include <stdint.h>


#define RCC_APB2ENR (*(volatile uint32_t*)0x40021018)


typedef struct {
	uint32_t  GPIOA_CRL;
	uint32_t GPIOA_CRH;
	uint32_t GPIOA_IDR;
	uint32_t GPIOA_ODR;
}GPIO_typedef;
#define GPIO_A ((GPIO_typedef *) 0x40010800)
void GPIO_init()
{
	 RCC_APB2ENR |= (1 << 2);
	 GPIO_A->GPIOA_CRL &= ~(0xFFF<<0);
	 GPIO_A->GPIOA_CRL |= (0x2<<0)|(0x2<<4)|(0x2<<8);



}

void vLED0Task (void *PvPara)
{
	while(1)
	{
		GPIO_A->GPIOA_ODR^=(1<<0);
		vTaskDelay(pdMS_TO_TICKS(200));

	}
}

void vLED1Task(void *PvPara)
{
	while(1)
	{
		GPIO_A->GPIOA_ODR^=(1<<1);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}


void vLED2Task(void *PvPara)
{
	while(1)
	{
		GPIO_A->GPIOA_ODR^=(1<<2);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

int main(void)
{
	GPIO_init();

	xTaskCreate(vLED0Task,"LED0",128, NULL, 1, NULL);
	xTaskCreate(vLED1Task ,"LED1",128, NULL, 1, NULL);
	xTaskCreate(vLED2Task,"LED2",128, NULL, 1, NULL);

	vTaskStartScheduler();
   while(1)
   {

   }
}
