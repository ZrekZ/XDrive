#include "stm32f303xc.h"
#include "spi.h"

void Spi1Clk_enable(void)	// Включение тактирования SPI1
{
	RCC->APB2ENR|=0x00001000;
}

void Spi2Clk_enable(void)	// Включение тактирования SPI2
{
    RCC->APB1ENR|=0x00004000;
}

void Spi3Clk_enable(void)	// Включение тактирования SPI3
{
	RCC->APB1ENR|=0x00008000;
}

void Spi2Init (void)	// Инициализация SPI2
{
	SPI2->CR1=0x0024;
	SPI2->CR2=0x0F0C;
}

void Spi2_enable (void)	// Включение SPI2
{
	SPI2->CR1|=0x0040;
}

void Spi2Start(void)
{
	Spi2Clk_enable();
	Spi2Init();
	Spi2_enable();
}

unsigned short SPI2_SR (unsigned short data)
{
	SPI2->DR=data;
	while ((SPI2->SR&0x0001)==0x00){;}
	data=SPI2->DR;
	return data;
}


