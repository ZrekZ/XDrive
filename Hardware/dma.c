
#include "stm32f303xc.h"
#include "dma.h"
#include "timers.h"
#include "software/pi_reg.h"


unsigned short spiSendReg[16]={0};
unsigned short spiReceiveReg=0;
// i2cSendArr: 0 - старший байт адреса
//			   1 - младший байт адреса
//			   2 - старший байт данных
//			   3 - младший байт данных
//unsigned char i2cReceiveArr[2]={0}, i2cSendArr[4]={0};
unsigned short adcReceiveArr[5]={0};
unsigned short adcSequenceArr[15]={0};

extern unsigned char test_drv;

void Dma1Start(void)
{
	Dma1Clk_enable();
	Dma1_Init();

	// Настройки для SPI2
	//set_Dma1MemPointer_short (4,&spiReceiveReg);
	//set_Dma1MemPointer_short (5,&spiSendReg[0]);
	//set_Dma1PeriphPointer_uint16t (4,&(SPI2->DR));
	//set_Dma1PeriphPointer_uint16t (5,&(SPI2->DR));
	//setDma1DataWidth(4,1);
	//setDma1DataWidth(5,16);

	// Настройки для I2C1
	/*
	set_Dma1MemPointer_char (6,&i2cSendArr[0]);
	set_Dma1MemPointer_char (7,&i2cReceiveArr[0]);
	set_Dma1PeriphPointer_uint32t (6,&(I2C1->TXDR));
	set_Dma1PeriphPointer_uint32t (7,&(I2C1->RXDR));
	setDma1DataWidth(6,2);
	setDma1DataWidth(7,2);
	*/

	// Настройки для ADC1
	/*
	set_Dma1MemPointer_short(1,&adcReceiveArr[0]);
	set_Dma1PeriphPointer_uint32t (1,&(ADC1->DR));
	setDma1DataWidth(1,4);
	*/
}

void Dma1_Init (void)
{
	// Настройки для SPI2
	//DMA1_Channel4->CCR=0x00001520;
	//DMA1_Channel5->CCR=0x000015B0;

	// Настройки для I2C1
	/*
	DMA1_Channel6->CCR=0x00001290;
	DMA1_Channel7->CCR=0x00001280;
	*/

	// Настройки для ADC1
	DMA1_Channel1->CCR=0x00001682;
}

void Dma1Clk_enable (void)
{
	RCC->AHBENR|=0x00000001;
}

void setDma1DataWidth(unsigned char dma_num,unsigned int width)  // Количество принимаемых данных
{
	switch (dma_num)
	{
		case 1: DMA1_Channel1->CNDTR=(uint32_t)width;break;
		case 2: DMA1_Channel2->CNDTR=(uint32_t)width;break;
		case 3: DMA1_Channel3->CNDTR=(uint32_t)width;break;
		case 4: DMA1_Channel4->CNDTR=(uint32_t)width;break;
		case 5: DMA1_Channel5->CNDTR=(uint32_t)width;break;
		case 6: DMA1_Channel6->CNDTR=(uint32_t)width;break;
		case 7: DMA1_Channel7->CNDTR=(uint32_t)width;break;
	}
}

void set_Dma1MemPointer_short (unsigned char dma_num, unsigned short* addr) // канал DMA, адрес - указатель на ячейку памяти unsigned short
{
	switch (dma_num)
	{
		case 1: DMA1_Channel1->CMAR=(uint32_t)addr;break;
		case 2: DMA1_Channel2->CMAR=(uint32_t)addr;break;
		case 3: DMA1_Channel3->CMAR=(uint32_t)addr;break;
		case 4: DMA1_Channel4->CMAR=(uint32_t)addr;break;
		case 5: DMA1_Channel5->CMAR=(uint32_t)addr;break;
		case 6: DMA1_Channel6->CMAR=(uint32_t)addr;break;
		case 7: DMA1_Channel7->CMAR=(uint32_t)addr;break;
	}
}

void set_Dma1MemPointer_char (unsigned char dma_num, unsigned char* addr) // канал DMA, адрес - указатель на ячейку памяти unsigned char
{
	switch (dma_num)
	{
		case 1: DMA1_Channel1->CMAR=(uint32_t)addr;break;
		case 2: DMA1_Channel2->CMAR=(uint32_t)addr;break;
		case 3: DMA1_Channel3->CMAR=(uint32_t)addr;break;
		case 4: DMA1_Channel4->CMAR=(uint32_t)addr;break;
		case 5: DMA1_Channel5->CMAR=(uint32_t)addr;break;
		case 6: DMA1_Channel6->CMAR=(uint32_t)addr;break;
		case 7: DMA1_Channel7->CMAR=(uint32_t)addr;break;
	}
}

void set_Dma1PeriphPointer_uint16t (unsigned char dma_num, uint16_t volatile* addr)  // канал DMA, адрес  - указатель на ячейку периферии
{
	 switch (dma_num)
	{
		case 1: DMA1_Channel1->CPAR=(uint32_t)addr;break;
		case 2: DMA1_Channel2->CPAR=(uint32_t)addr;break;
		case 3: DMA1_Channel3->CPAR=(uint32_t)addr;break;
		case 4: DMA1_Channel4->CPAR=(uint32_t)addr;break;
		case 5: DMA1_Channel5->CPAR=(uint32_t)addr;break;
		case 6: DMA1_Channel6->CPAR=(uint32_t)addr;break;
		case 7: DMA1_Channel7->CPAR=(uint32_t)addr;break;
	}
}

void set_Dma1PeriphPointer_uint32t (unsigned char dma_num, uint32_t volatile* addr)  // канал DMA, адрес  - указатель на ячейку периферии
{
	 switch (dma_num)
	{
		case 1: DMA1_Channel1->CPAR=(uint32_t)addr;break;
		case 2: DMA1_Channel2->CPAR=(uint32_t)addr;break;
		case 3: DMA1_Channel3->CPAR=(uint32_t)addr;break;
		case 4: DMA1_Channel4->CPAR=(uint32_t)addr;break;
		case 5: DMA1_Channel5->CPAR=(uint32_t)addr;break;
		case 6: DMA1_Channel6->CPAR=(uint32_t)addr;break;
		case 7: DMA1_Channel7->CPAR=(uint32_t)addr;break;
	}
}



void Dma1ChannelOnOffState (unsigned char dma_num,unsigned char state)
{
	if (state==0) // выключить канал
	{
		switch (dma_num)
		{
			case 1: DMA1_Channel1->CCR&=0xFFFFFFFE;break;
			case 2: DMA1_Channel2->CCR&=0xFFFFFFFE;break;
			case 3: DMA1_Channel3->CCR&=0xFFFFFFFE;break;
			case 4: DMA1_Channel4->CCR&=0xFFFFFFFE;break;
			case 5: DMA1_Channel5->CCR&=0xFFFFFFFE;break;
			case 6: DMA1_Channel6->CCR&=0xFFFFFFFE;break;
			case 7: DMA1_Channel7->CCR&=0xFFFFFFFE;break;

		}
	}
	else if (state==1)  // включить канал
	{
		switch (dma_num)
		{
			case 1: DMA1_Channel1->CCR|=0x00000001;break;
			case 2: DMA1_Channel2->CCR|=0x00000001;break;
			case 3: DMA1_Channel3->CCR|=0x00000001;break;
			case 4: DMA1_Channel4->CCR|=0x00000001;break;
			case 5: DMA1_Channel5->CCR|=0x00000001;break;
			case 6: DMA1_Channel6->CCR|=0x00000001;break;
			case 7: DMA1_Channel7->CCR|=0x00000001;break;
		}
	}
}

/*
void DMA1_Channel1_IRQHandler(void)
{

    ADC1->CR2&=~0x00000001;		// Включение АЦП
	//GPIOB->ODR|=0x00000040;
	TIM2->SR&=~0x0004;

	DMA1->IFCR|=0x00000002;		// Сброс флага окончания передачи по DMA

	Dma1ChannelOnOffState(1,0); // Отключение канала DMA

    ReadStartKey();

	DriverProcess();

	setDma1DataWidth(1,4); // Задаем кол-во передаваемых данных по DMA

	DMA1->IFCR|=0x00000001;
	Dma1ChannelOnOffState(1,1); // Включение канала DMA
    //GPIOB->ODR&=~0x00000040;
	//GPIOB->ODR&=~0x00000200;
	ADC1->CR2|=0x00000001;		// Включение АЦП
}
*/