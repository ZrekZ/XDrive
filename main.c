
// Плата двигателя WFU - ПА

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "hardware\hardware.h"
#include "stm32f303xc.h"
#include "hardware\spi.h"
#include "hardware\switches.h"
#include "hardware\irq.h"
#include "hardware\Timers.h"
#include "hardware\dma.h"
#include "hardware\i2c.h"
#include "software\menu.h"
#include "software\pi_reg.h"
#include "hardware\adc.h"
#include "hardware\dac.h"
#include "hardware\usart.h"
#include "software\transmission.h"
#include "crc.h"

extern unsigned char update;
extern unsigned short dig2show;

// Плата двигателя WFU - ПА


int main(void)
{
	PORTA_Enable();
	PORTB_Enable();
	PORTC_Enable();
	PORTF_Enable();
	PortsInit();

	StartCRC();
	//Dma1Start();
	Spi2Start();
	Usart2Start();
	//Dac2Start ();

	EnableIRQs();

	//Dma1ChannelOnOffState(4,1);
	//Dma1ChannelOnOffState(5,1);
	//Dma1ChannelOnOffState(1,1);

	AdcStart();
	TimerStart(1); // Таймер ШИМ двигателя и запуска АЦП
	//TimerStart(14);
	TimerStart(6);
	//SysTickTimerInit();
	OPAMP_Init(1);
	OPAMP_Init(2);

	//I2C2Start();

	InitUserParameters();
	//while (!(ReadParamsFromMem(1,9))){;}
	//while (!(ReadParamsFromMem(30,24))){;}
	ADCStart(1);
	ADCStart(3);

  __enable_interrupt();
  /* Infinite loop */
  while (1)
  {
	//GPIOB->ODR|=0x00000100; // - для отслеживания процессов PB8
	//GPIOB->ODR|=0x00000040; // - для отслеживания процессов PB6

	//menu();
	//ShowMenu();

	SaveParamFromQueue();
	SPITransmission();
	ParsingSPIData();
	ExtractDRVData();
	PackDRVData();
	DrvTransmitControl ();

	while (TIM6->CNT<(TIM6->ARR>>1)){;}
	TIM6->CNT=0;
  }
}

/*****************************END OF FILE****/
