
#include "stm32f303xc.h"
#include "crc.h"


void StartCRC (void)
{
	CRCClkEnable();
	CRCInit();
}

void CRCClkEnable (void)
{
	RCC->AHBENR |= 0x00000040;
}

void CRCInit (void)
{
	CRC->CR = 0x00000008; // polinomial size: 16bit
	CRC->INIT = 0xAA0000AA;
}

void CRCReset (void)
{
	CRC->CR|=0x00000001;
}

unsigned short CRC_Calculate (unsigned char* arr, unsigned char begin_num, unsigned char amount)
{

	unsigned short temp,i2;
	unsigned char coll = amount+begin_num-1;
	
	
	for (unsigned char i = begin_num; i <= coll; i+=2)
	{
		
		i2 = i+1;
		if (i2 > coll) temp = ((unsigned short)*(arr+i))<<8;
		else temp = (((unsigned short)*(arr+i))<<8)|*(arr+i2);
		
		CRC->DR = temp;
		
	}
	
	return CRC->DR;
}