#include "Timers.h"
#include "stm32f303xc.h"
#include "intrinsics.h"

#define CPU_FREQ 72000000

unsigned char update=0;

unsigned short psc=0,arr=0;

//************************************************
// Функции таймера 6 - BASIC timer
//************************************************

void TimerStart (unsigned char timerNum)
{
	TimerClkEnable(timerNum);
	TimerInit(timerNum);
	TimerEnable(timerNum);
}

void TimerEnable (unsigned char timerNum)	// Запуск таймера - установка бита Counter enable
{
	switch (timerNum)
	{

		case 1: TIM1->CR1|=0x00000001; break;
		case 2:  TIM2->CR1|=0x00000001; break;
		case 6:  TIM6->CR1|=0x00000001; break;
		//case 14: TIM14->CR1|=0x0001; break;
		//case 19: TIM19->CR1|=0x0001; break;
	}
}

void TimerClkEnable(unsigned char  timerNum) // Включение тактирования таймера
{
	switch (timerNum)
	{
		case 1: RCC->APB2ENR|=0x00000800;break;
		case 2: RCC->APB1ENR|=0x00000001; break;
		case 3: RCC->APB1ENR|=0x00000002; break;
		case 4: RCC->APB1ENR|=0x00000004; break;
		case 5: RCC->APB1ENR|=0x00000008; break;
		case 6: RCC->APB1ENR|=0x00000010; break;
		case 7: RCC->APB1ENR|=0x00000020; break;
		case 8:break;
		case 9:break;
		case 10:break;
		case 11:break;
		case 12: RCC->APB1ENR|=0x00000040; break;
		case 13: RCC->APB1ENR|=0x00000080; break;
		case 14: RCC->APB1ENR|=0x00000100; break;
		case 15: RCC->APB2ENR|=0x00010000; break;
		case 16: RCC->APB2ENR|=0x00020000; break;
		case 17: RCC->APB2ENR|=0x00040000; break;
		case 18: RCC->APB1ENR|=0x00000200; break;
		case 19: RCC->APB2ENR|=0x00080000; break;
	}
}

void TimerInit (unsigned char timerNum)
{
	switch (timerNum)
	{
	case 1:
		TIM1->CR1=0x00;
		TIM1->CR2=0x20; // Update Event Selected as TRGO
		TIM1->SMCR=0x00;
		TIM1->CCMR1=0x0068; // PWM mode 1
		TIM1->CCER=0x04;
		TIM1->ARR=4095;
		TIM1->PSC=0;
		TIM1->RCR=0x00;
		TIM1->CCR1=0x00;
		TIM1->BDTR|=0x8000;
	break;
	case 2: // ШИМ двигателя
		TIM2->CR1=0x00;
		TIM2->CR2=0x00;
		TIM2->SMCR=0x00;
		TIM2->EGR|=0x44;
		TIM2->CCMR1=0x6800;
		TIM2->CCMR2=0x0068;
		TIM2->CCER=0x0110; // CH 3 , CH2 not active
		TIM2->PSC=0;
		TIM2->ARR=4095;
		TIM2->CCR3=0;
		TIM2->CCR2=1;
	break;
	case 6: // Контролирует программный цикл
		TIM6->CR1=0x00000000;	// CONTROL REGISTER 1
		TIM6->CR2=0x00000000;   // CONTROL REGISTER 2
		TIM6->DIER=0x00000000;  // DMA/Interrupt enable register
		TimerParams(200); // Период таймера в микросекундах
		TIM6->PSC=psc;	// PRESCALER REGISTER
		TIM6->ARR=arr;  // AUTO RELOAD REGISTER
	break;
	/*
	case 14: // Таймер по которому срабатывает АЦП
		TIM14->CR1=0x0080;
		TIM14->DIER=0x01;
		TIM14->CCMR1=0x0000;
		TIM14->CCER=0x0000;
		TimerParams(50); // Период таймера в микросекундах
		TIM14->PSC=psc;
		TIM14->ARR=arr;
		TIM14->CCR1=0;
	break;
	*/

	}
}

//****************** Таймер 2 ***********************
void Val2TimerCCR (unsigned short pwm)
{
	TIM1->CCR1=pwm;
}

//******************* END ****************************

//****************** Таймер 14 ***********************
/*
void Timer14Stop (void)
{
	TIM14->CR1&=0xFFFE;
}

void TIM14_IRQHandler ()
{
	TIM14->SR&=0xFFFE;
	ADC1->CR2|=0x00000001;
}
*/

//******************* END ****************************

//****************** Таймер 6 ************************

//******************* END ****************************


//******************************************************
// Фунция рассчета AUTORELOAD и PRESCALER регистров для таймеров
// на выходе: psc - значение регистра PRESCALER
//            arr - значение регистра ARR
void TimerParams (unsigned int usecs) // Функция рассчета
{
	unsigned int cycles;

	psc=0;arr=0;
	cycles=usecs*(CPU_FREQ/1000000);
	if (cycles>65535)
	{
		while (cycles>65535)
		{
			cycles=cycles/2;
			psc++;
		}

		psc=(1<<psc);
		psc--;
	}

	arr=cycles;
}

//************** SysTickTimer ********************
void SysTickTimerInit(void)
{
	SysTick->CTRL=0x00000004;
}

void SysTickTimerLoad (unsigned int u_secs)
{
	SysTick->LOAD=(u_secs*72)-1; // При частоте 72 000 000 Мгц множитель 72
}

void SysTickStart(unsigned int u_secs)
{
	SysTick->LOAD=(u_secs*72)-1; // При частоте 72 000 000 Мгц множитель 72
	SysTick->CTRL|=1; // Запуск таймера SysTick
}

unsigned char getSysTickStatus (void)
{
	unsigned char end;

	if ((SysTick->CTRL&0x00010000)==0x00010000) // Проверка таймера на переполнение
	{
		end=1;
		SysTick->CTRL&=0xFFFFFFFE; // Остановка таймера SysTick
	}
	else end=0;

	return end;
}

void SysTickCount (unsigned int u_secs)
{
	SysTick->LOAD=(u_secs*72)-1; // При частоте 72 000 000 Мгц множитель 72

	SysTick->CTRL|=1; // Запуск таймера SysTick
	while ((SysTick->CTRL&0x00010000)==0x00000000){;}
	SysTick->CTRL&=0xFFFFFFFE; // Остановка таймера SysTick
}
//************************************************
/*

void TIM6_DAC1_IRQHandler()	// Прерывание по обновлении счетчика
{
    TIM6->SR&=0xFFFE;	// Сброс флага обновления
	update=1;
}


void Timer19Start (void)
{
	TimerClkEnable(19);
	Timer19Init();
	TimerEnable(19);
}

void Timer19Init (void) // Таймер обновления ПИ-регулятора и АЦП с пересылкой по DMA
{
	TIM19->CR1=0x0080;
	TIM19->CR2=0x0070;
	TIM19->SMCR=0x0008;
	TIM19->DIER=0x00;
	//TIM19->EGR=0x0000;
	TIM19->CCMR2=0x6800;
	TIM19->CCER=0x1000;
	TimerParams(50); // Период таймера в микросекундах
	TIM19->PSC=psc;
	TIM19->ARR=arr;
	TIM19->CCR4=(TIM19->ARR/2);
}
*/
