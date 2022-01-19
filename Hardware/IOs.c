//***********************************
// Инициализация портов ввода-вывода
//***********************************

#include "IOs.h"
#include "stm32f303xc.h"

void PortsInit(void)
{
	//*********************************************
	// GPIO Port mode register
	// GPIOx_MODER x=A..F, используется 2 бита на 1 пин, пин 0 - биты 1,0 и т.д.
		// 00: Input mode (reset state)
		// 01: General purpose output mode
		// 10: Alternate function mode
		// 11: Analog mode

	GPIOA->MODER=0x2815F0FC;
	GPIOB->MODER=0xAA00A28C;
	GPIOC->MODER=0x08000000;
	//GPIOD->MODER=0x00000000;
	//GPIOE->MODER=0x00000000;
	//GPIOF->MODER=0x00000000;

	//*********************************************
	// GPIO port output type register
	// GPIOx_OTYPER x=A..F, используется 1 бит на 1 пин, используются биты с 0 по 15
		//0: Output push-pull (reset state)
		//1: Output open-drain

	GPIOA->OTYPER=0x00000000;
	GPIOB->OTYPER=0x00000000;
	//GPIOC->OTYPER=0x00000000;
	//GPIOD->OTYPER=0x00000000;
	//GPIOE->OTYPER=0x00000000;
	//GPIOF->OTYPER=0x00000000;

	//*********************************************
	// GPIO port output speed register
	// GPIOx_OSPEEDR x = A..F, используется 2 бита на 1 пин, пин 0 - биты 1,0 и т.д.
		//x0: Low speed
		//01: Medium speed
		//11: High speed

	GPIOA->OSPEEDR=0x00000000;
	GPIOB->OSPEEDR=0xFF00F3C0;
	GPIOC->OSPEEDR=0x0C000000;
	//GPIOD->OSPEEDR=0x00000000;
	//GPIOE->OSPEEDR=0x00000000;
	//GPIOF->OSPEEDR=0x0000F000;

	//*********************************************
	// GPIO port pull-up/pull-down register
	// GPIOx_PUPDR x=A..F, используется 2 бита на 1 пин, пин 0 - биты 1,0 и т.д.
		//00: No pull-up, pull-down
		//01: Pull-up
		//10: Pull-down
		//11: Reserved

	 GPIOA->PUPDR=0x00000000;
	 GPIOB->PUPDR=0x00000000;
	 GPIOC->PUPDR=0x00000000;
	 //GPIOD->PUPDR=0x00000000;
	 //GPIOE->PUPDR=0x00000000;
	 //GPIOF->PUPDR=0x00000000;

	//*********************************************
	// GPIO alternate function low register (пины 0-7)
	// GPIOx_AFRL x=A..E, используется 4 бита на 1 пин, пин 0 - биты 0,1,2,3 и
		//0000: AF0		//1000: AF8 (Ports A, B and D only)
		//0001: AF1		//1001: AF9 (Ports A, B and D only)
		//0010: AF2		//1010: AF10 (Ports A, B and D only)
		//0011: AF3		//1011: AF11 (Ports A, B and D only)
		//0100: AF4     //1100: AF12 (Ports A, B and D only)
		//0101: AF5     //1101: AF13 (Ports A, B and D only)
		//0110: AF6     //1110: AF14 (Ports A, B and D only)
		//0111: AF7     //1111: AF15 (Ports A, B and D only)

	//*********************************************
	//GPIO alternate function high register
	//GPIOx_AFRH x = A..F, используется 4 бита на 1 пин, пин 8 - биты 0,1,2,3

		//0000: AF0		1000: AF8 (Ports A, B and D only)
		//0001: AF1		1001: AF9 (Ports A, B and D only)
		//0010: AF2		1010: AF10 (Ports A, B and D only)
		//0011: AF3		1011: AF11 (Ports A, B and D only)
		//0100: AF4		1100: AF12 (Ports A, B and D only)
		//0101: AF5		1101: AF13 (Ports A, B and D only)
		//0110: AF6		1110: AF14 (Ports A, B and D only)
		//0111: AF7		1111: AF15 (Ports A, B and D only)

		GPIOA->AFR[0]=0x00000000;
		GPIOA->AFR[1]=0x00000000;

		GPIOB->AFR[0]=0x44077000;
		GPIOB->AFR[1]=0x55550000;

		GPIOC->AFR[0]=0x00000000;
		GPIOC->AFR[1]=0x00400000; // TIM1_CH1N

}

void PORTA_Enable(void) // I/O PortA clock enable
{
	RCC->AHBENR|=0x00020000;
}
void PORTB_Enable(void) // I/O PortB clock enable
{
	RCC->AHBENR|=0x00040000;
}
void PORTC_Enable(void) // I/O PortC clock enable
{
	RCC->AHBENR|=0x00080000;
}
void PORTD_Enable(void) // I/O PortD clock enable
{
	RCC->AHBENR|=0x00100000;
}
void PORTE_Enable(void) // I/O PortE clock enable
{
	RCC->AHBENR|=0x00200000;
}
void PORTF_Enable(void)	// I/O PortF clock enable
{
	RCC->AHBENR|=0x00400000;
}

void Pwm2Drv (unsigned char on) // Сигнал EMF
{
	if (on==0)
	{
		GPIOA->ODR&=(~0x00000002);
	}
	else if (on==1)
	{
		GPIOA->ODR|=0x00000002;
	}
}

void ClosingVT (unsigned char on) // Сигнал TER
{
	if (on==0)
	{
		GPIOA->ODR&=(~0x00000001);
	}
	else if (on==1)
	{
		GPIOA->ODR|=0x00000001;
	}
}