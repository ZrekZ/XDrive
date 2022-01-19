/****************************
** Каналов для преобразования всего: 7
** каналы 1.1 и 1.2 - температура, преобразования выполняются как инжектированные после регулярных,
** соответственно сохраняются в ADC1->JDR1 и ADC1->JDR2.
**
** АЦП 1 и 2 работают одновременно по триггеру от внешней синхронизации или сигнала TIM16->CH1 (подается с вывода PA11 на вывод PB11)
** АЦП 3 и 4 работают одновременно и срабатывают от прерывания на ножке PB11. Прерывание также вызывается сигналом синхронизации, либо таймером TIM16->CH1.
** АЦП 1,2,3,4 работают одновременно.
**
** У АЦП1 преобразуются каналы 3 и 4, у АЦП2 преобразуется канал 3.
** У АЦП3 преобразуется канал 1, у АЦП4 преобразуется канал 3.
** Преобразованные данные АЦП1 передаются по DMA в массив adcReceiveArr[].
** Во время 2-х преобразований АЦП1 два раза преобразуется один и тот же канал на АЦП2.
** После окончания передачи результатов АЦП1, вызывается прерывание по окончании передачи по DMA1_1.
** В прерывании DMA1_1 стоит ожидание окончания выполнения АЦП3,4 (запущенных из прерывания одновременным сигналом)  
** после чего запускается выполнение программы регуляторов.
**
****************************/

#include "stm32f303xc.h"
#include "adc.h"

#include "pi_reg.h"

extern unsigned short adcReceiveArr[5];

void AdcStart (void)
{
	AdcClk_enable();
	AdcCalibrate();
	AdcEnable();
	AdcInit();
	//Adc1Enable();
}

void AdcClk_enable (void) // Включение тактирования АЦП
{
	RCC->AHBENR|=0x30000000;
}

void AdcCalibrate (void) // Калибровка АЦП
{
	//RCC->CFGR2=0x00002100;
	ADC12_COMMON->CCR=0x00010001;
	ADC34_COMMON->CCR=0x00010000;
	
	ADC1->CR&=0xCFFFFFFF; // ADVREGEN=0x00
	ADC2->CR&=0xCFFFFFFF;
	ADC3->CR&=0xCFFFFFFF;
	ADC4->CR&=0xCFFFFFFF;

	ADC1->CR|=0x10000000; // ADVREGEN=0x01 - Adc voltage regulator enable
	ADC2->CR|=0x10000000;
	ADC3->CR|=0x10000000;
	ADC4->CR|=0x10000000;
	
	ADC1->CR&=0xBFFFFFFF;; // ADCALDIF=0x00 - launch a calibration in Single-ended inputs Mode
	ADC2->CR&=0xBFFFFFFF;
	ADC3->CR&=0xBFFFFFFF;
	ADC4->CR&=0xBFFFFFFF;
	
	for (unsigned short i=0;i<5000;i++) // Пауза для установления напряжения регулятора
	{
		asm("nop");
	}
	
	//ADC1->CR|=0x00000001;
	//ADC2->CR|=0x00000001;
	//ADC3->CR|=0x00000001;
	//ADC4->CR|=0x00000001;
	
	ADC1->CR|=0x80000000; // ADCAL=1 - Запуск калибровки АЦП
	while ((ADC1->CR&0x80000000)==0x80000000){;} // Ждем пока калибровка закончится
	ADC2->CR|=0x80000000;
	while ((ADC2->CR&0x80000000)==0x80000000){;}
	ADC3->CR|=0x80000000;
	while ((ADC3->CR&0x80000000)==0x80000000){;}
	ADC4->CR|=0x80000000;
	while ((ADC4->CR&0x80000000)==0x80000000){;}
}

void AdcInit (void)
{
	// Настройка АЦП 1,2
	ADC1->CFGR=0x00001640; // Event 9
	ADC1->IER = 0x00000004; // Прерывание по окончании преобразования
	ADC1->SMPR1=0x00000C00; // adc1.3
	ADC2->SMPR1=0x00000C00; // adc2.3
	ADC1->SQR1=0x000000C0; //
	ADC2->SQR1=0x000000C0; //
	//ADC1->JSQR=0x00008101;
	
	// Настройка порта, с которого берется сигнал синхронизации
	//SYSCFG->EXTICR[2]&=0xFFFF0FFF; // ОБнуление битов, отвечающих за EXTI11
	//SYSCFG->EXTICR[2]|=0xFFFF1FFF; // EXTI11 берется с порта PB11 - для работы АЦП достаточно только этого
	
	// Настройка АЦП 3,4
	ADC3->CFGR=0x00001640; // Event 9
	ADC3->SMPR1=0x00000028; // adc3.1
	ADC3->SMPR2=0x0000000; 
	ADC3->SQR1=0x00000040;
}

void AdcEnable (void)
{
	ADC1->CR|=0x00000001; // Включение АЦП
	while ((ADC1->ISR&0x00000001)==0x00){;} // Ожидание готовности АЦП
	ADC2->CR|=0x00000001;
	while ((ADC2->ISR&0x00000001)==0x00){;}
	ADC3->CR|=0x00000001;
	while ((ADC3->ISR&0x00000001)==0x00){;}
	ADC4->CR|=0x00000001;
	while ((ADC4->ISR&0x00000001)==0x00){;}
}

void ADCStart(unsigned char num) // Запуск преобразования
{
	switch (num)
	{
		case 1: ADC1->CR|=0x00000004;break;
		case 2: ADC2->CR|=0x00000004;break;
		case 3: ADC3->CR|=0x00000004;break;
		case 4: ADC4->CR|=0x00000004;break;
	}
}


void ReadInjADC(void)
{
	if (ADC3->ISR&0x00000040)
	{
		injADC.temp1 = ADC3->JDR1;
		injADC.temp2 = ADC3->JDR2;
	}
}

void OPAMP_Init(unsigned char num)
{
	switch (num)
	{
		case 1:OPAMP1->CSR|=0x2D;break;
		case 2:OPAMP2->CSR|=0x404D;	break;
		case 3:OPAMP3->CSR|=0x002C;	break;
		case 4:break;
	}
}

void ADC1_2_IRQHandler(void)
{
	adcReceiveArr[0] = (unsigned short)ADC12_COMMON->CDR; // Отрицательный ток
	adcReceiveArr[1] = (unsigned short)(ADC12_COMMON->CDR>>16); // Положительный Ток
	adcReceiveArr[2] = (unsigned short)ADC3->DR;
	
	ReadStartKey();
	DriverProcess();
}