/****************************
** ������� ��� �������������� �����: 7
** ������ 1.1 � 1.2 - �����������, �������������� ����������� ��� ��������������� ����� ����������,
** �������������� ����������� � ADC1->JDR1 � ADC1->JDR2.
**
** ��� 1 � 2 �������� ������������ �� �������� �� ������� ������������� ��� ������� TIM16->CH1 (�������� � ������ PA11 �� ����� PB11)
** ��� 3 � 4 �������� ������������ � ����������� �� ���������� �� ����� PB11. ���������� ����� ���������� �������� �������������, ���� �������� TIM16->CH1.
** ��� 1,2,3,4 �������� ������������.
**
** � ���1 ������������� ������ 3 � 4, � ���2 ������������� ����� 3.
** � ���3 ������������� ����� 1, � ���4 ������������� ����� 3.
** ��������������� ������ ���1 ���������� �� DMA � ������ adcReceiveArr[].
** �� ����� 2-� �������������� ���1 ��� ���� ������������� ���� � ��� �� ����� �� ���2.
** ����� ��������� �������� ����������� ���1, ���������� ���������� �� ��������� �������� �� DMA1_1.
** � ���������� DMA1_1 ����� �������� ��������� ���������� ���3,4 (���������� �� ���������� ������������� ��������)  
** ����� ���� ����������� ���������� ��������� �����������.
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

void AdcClk_enable (void) // ��������� ������������ ���
{
	RCC->AHBENR|=0x30000000;
}

void AdcCalibrate (void) // ���������� ���
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
	
	for (unsigned short i=0;i<5000;i++) // ����� ��� ������������ ���������� ����������
	{
		asm("nop");
	}
	
	//ADC1->CR|=0x00000001;
	//ADC2->CR|=0x00000001;
	//ADC3->CR|=0x00000001;
	//ADC4->CR|=0x00000001;
	
	ADC1->CR|=0x80000000; // ADCAL=1 - ������ ���������� ���
	while ((ADC1->CR&0x80000000)==0x80000000){;} // ���� ���� ���������� ����������
	ADC2->CR|=0x80000000;
	while ((ADC2->CR&0x80000000)==0x80000000){;}
	ADC3->CR|=0x80000000;
	while ((ADC3->CR&0x80000000)==0x80000000){;}
	ADC4->CR|=0x80000000;
	while ((ADC4->CR&0x80000000)==0x80000000){;}
}

void AdcInit (void)
{
	// ��������� ��� 1,2
	ADC1->CFGR=0x00001640; // Event 9
	ADC1->IER = 0x00000004; // ���������� �� ��������� ��������������
	ADC1->SMPR1=0x00000C00; // adc1.3
	ADC2->SMPR1=0x00000C00; // adc2.3
	ADC1->SQR1=0x000000C0; //
	ADC2->SQR1=0x000000C0; //
	//ADC1->JSQR=0x00008101;
	
	// ��������� �����, � �������� ������� ������ �������������
	//SYSCFG->EXTICR[2]&=0xFFFF0FFF; // ��������� �����, ���������� �� EXTI11
	//SYSCFG->EXTICR[2]|=0xFFFF1FFF; // EXTI11 ������� � ����� PB11 - ��� ������ ��� ���������� ������ �����
	
	// ��������� ��� 3,4
	ADC3->CFGR=0x00001640; // Event 9
	ADC3->SMPR1=0x00000028; // adc3.1
	ADC3->SMPR2=0x0000000; 
	ADC3->SQR1=0x00000040;
}

void AdcEnable (void)
{
	ADC1->CR|=0x00000001; // ��������� ���
	while ((ADC1->ISR&0x00000001)==0x00){;} // �������� ���������� ���
	ADC2->CR|=0x00000001;
	while ((ADC2->ISR&0x00000001)==0x00){;}
	ADC3->CR|=0x00000001;
	while ((ADC3->ISR&0x00000001)==0x00){;}
	ADC4->CR|=0x00000001;
	while ((ADC4->ISR&0x00000001)==0x00){;}
}

void ADCStart(unsigned char num) // ������ ��������������
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
	adcReceiveArr[0] = (unsigned short)ADC12_COMMON->CDR; // ������������� ���
	adcReceiveArr[1] = (unsigned short)(ADC12_COMMON->CDR>>16); // ������������� ���
	adcReceiveArr[2] = (unsigned short)ADC3->DR;
	
	ReadStartKey();
	DriverProcess();
}