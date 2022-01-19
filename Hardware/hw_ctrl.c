#include "hw_ctrl.h"
#include "stm32f303xc.h"

unsigned char getMonitorStatus (void) // Состояние монитора питания
{
	static unsigned char monitor=0;
	static unsigned short counter=0;

	if ((GPIOC->IDR&GPIO_IDR_13)==GPIO_IDR_13)
	{
		counter++;
		{
			if (counter>=500)
			{
				counter=499;
				monitor=1;
			}
		}
	}
	else if (counter>0)
	{
		counter--;
	}
	else if (counter==0 ) {monitor=0;counter=0;}

	return monitor;
}

void setFan (unsigned char command) // Включение/отключение вентилятора
{
	if (command==1)
	{
		GPIOC->ODR|=GPIO_ODR_14;
	}
	else if (command==0)
	{
		GPIOC->ODR&=(~GPIO_ODR_14);
	}
}

void setSoftStart (unsigned char command)  // Плавный пуск
{
	if (command==1)
	{
		GPIOC->ODR|=GPIO_ODR_15;
	}
	else if (command==0)
	{
		GPIOC->ODR&=(~GPIO_ODR_15);
	}
}

void setMotorOn (unsigned char command) // Влкючение/отключение двигателя
{
	if (command==1)
	{
		GPIOE->ODR|=GPIO_ODR_8;
	}
	else if (command==0)
	{
		GPIOE->ODR&=(~GPIO_ODR_8);
	}
}


void setCoolingOn (unsigned char command) // Включение/отключение охлаждения
{
	if (command==1)
	{
		GPIOB->ODR|=GPIO_ODR_14;
	}
	else if (command==0)
	{
		GPIOB->ODR&=(~GPIO_ODR_14);
	}
}

unsigned char getFlowStatus (void) // Состояние датчика потока
{
	static unsigned char flow=0;
	static unsigned short counter=0;

	if ((GPIOB->IDR&GPIO_IDR_15)==GPIO_IDR_15)
	{
		counter++;
		{
			if (counter>=500)
			{
				counter=499;
				flow=1;
			}
		}
	}
	else if (counter>0)
	{
		counter--;
	}
	else if (counter==0 ) {flow=0;counter=0;}

	return flow;
}

unsigned char getSwitchStatus (unsigned char mode) // Состояние кнопки
{
	static unsigned char b_switch=0;
	static unsigned short counter=0;
	static unsigned char button_on=0,on=0,start=0;

	if ((GPIOD->IDR&GPIO_IDR_8)==GPIO_IDR_8) // Двунаправленный антидребезг
	{
		counter++;	// Если кнопку нажали, то счет вперед
		{
			if (counter>=500) // Если после этого кнопка еще нажата,то
			{
				counter=499;
				b_switch=1; // Сообщение - кнопка нажата
			}
		}
	}
	else if (counter>0) // Если кнопка не нажата, но до этого была нажата
	{
		counter--; // то считаем назад
	}
	else if (counter==0 ) {b_switch=0;counter=0;} // Сообщение - кнопка не нажата

	if (b_switch==1)
	{
		if (on==0) {on=1;button_on=1;}
		else if (on==2) {on=3;button_on=3;}
	}
	else if (b_switch==0)
	{
		if (on==1) {on=2;button_on=2;}
		else if (on==3) {on=0;button_on=0;}
	}

	switch (mode)
	{
	case 0:		// Режим 2Т
			 if (button_on==1) {on=3;start=1;}
			 else if (button_on==0) {start=0;}
		break;
	case 1:		// Режим 4Т
			if (button_on==2) {start=1;}
			else if (button_on==0){start=0;}
		break;
	case 2:     // Точечный
			if (button_on==1) {on=3;start=1;}
			else if (button_on==0) {start=0;}
		break;
	case 3:		// Интервальный
		    if (button_on==1) {on=3;start=1;}
			else if (button_on==0) {start=0;}
		break;
	}
	return start;
}

void SetValve1 (unsigned char com)
{
	if (com==1){GPIOA->ODR|=GPIO_ODR_9;}
	else if (com==0){GPIOA->ODR&=(~GPIO_ODR_9);}
}

void SetValve2 (unsigned char com)
{
	if (com==1){GPIOA->ODR|=GPIO_ODR_8;}
	else if (com==0){GPIOA->ODR&=(~GPIO_ODR_8);}
}
void SetRelay (unsigned char com)
{
	if (com==1){GPIOA->ODR|=GPIO_ODR_10;}
	else if (com==0){GPIOA->ODR&=(~GPIO_ODR_10);}
}





