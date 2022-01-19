
#include "stm32f303xc.h"
#include "irq.h"

void EnableIRQs (void)
{
	//NVIC_EnableIRQ(TIM14_IRQn);
	//NVIC_SetPriority(TIM14_IRQn,5);

	//NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	//NVIC_SetPriority(DMA1_Channel1_IRQn,7);

	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn,4);
	
	NVIC_EnableIRQ(ADC1_2_IRQn );
	NVIC_SetPriority(ADC1_2_IRQn ,5);
}