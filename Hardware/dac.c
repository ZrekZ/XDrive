
#include "stm32f303xc.h"
#include "dac.h"


//************ DAC 1 *****************
void Dac1CLK_enable (void)
{
	RCC->APB1ENR|=0x20000000;
}

void Dac1Init (void)
{
	DAC1->CR=0x00020002;
}

void Dac1Enable (void)
{
	DAC1->CR|=0x00000001;
}

void Dac1Start (void)
{
	Dac1CLK_enable();
	Dac1Init();
	Dac1Enable();
}

void Val2Dac1 (unsigned short val)
{
	 DAC1->DHR12R1=val;
}
//************************************


//************ DAC 2 *****************
/*
void Dac2CLK_enable (void)
{
	RCC->APB1ENR|=0x04000000;
}
void Dac2Init (void)
{
	DAC2->CR=0x00020002;
}

void Dac2Enable (void)
{
	DAC2->CR|=0x00000001;
}

void Dac2Start (void)
{
	Dac2CLK_enable();
	Dac2Init();
	Dac2Enable();
}
#pragma optimize=none
void Val2Dac2 (unsigned short val)
{
	 DAC2->DHR12R1=val;
}
*/
//************************************






