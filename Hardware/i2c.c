#include "stm32f303xc.h"
#include "i2c.h"
#include "dma.h"
#include "software/menu.h"

unsigned char i2cReceiveArr[2]={0}, i2cSendArr[4]={0};

unsigned char numQueue[8]={0};
unsigned short valQueue[8]={0};
unsigned char q_pointerEnd=0, q_pointerBeg=0;
unsigned char i2c_busy=0;

void I2c2Clk_enable (void)	// ������������ I2C1
{
	// ������������ I2C1
	//RCC->CFGR3|=0x00000010;	// ������������ �� SYSCLK
	//RCC->APB1ENR|=0x00200000;

	// ������������ I2C2
	RCC->CFGR3|=0x00000020;	// ������������ �� SYSCLK
	RCC->APB1ENR|=0x00400000;
}

void I2c2Init (void)	// ������������� ���������
{
	I2C2->CR1=0x00000000;
	I2C2->CR2=0x020000A0;
	I2C2->TIMINGR=0x50330309;
	I2C2->TIMEOUTR=0x00;
}

void I2c2_enable (void)
{
	I2C2->CR1|=0x00000001;
}

void I2C2Start (void)	// ������ I2C
{
	I2c2Clk_enable();
	I2c2Init();
	I2c2_enable();
}

void I2c2WriteAddr (void)
{

	I2C2->CR2&=0xFFFFFBFF;	// �������� ����� ������
	I2C2->CR2&=0xFF00FFFF;	// �������� ���-�� ����������� ����
	I2C2->CR2|=0x02020000;	// ���������� ������������ ���� - 2
	I2C2->CR2|=0x00002000;	// ����� ������
	I2C2->TXDR=i2cSendArr[0];
	while ((I2C2->ISR&0x00000001)==0){;}
	I2C2->TXDR=i2cSendArr[1];
	while ((I2C2->ISR&0x00000001)==0){;}
}

void I2c2WriteData (void)
{
	I2C2->CR2&=0xFFFFFBFF;	// �������� ����� ������
	I2C2->CR2&=0xFF00FFFF;	// �������� ���-�� ����������� ����
	I2C2->CR2|=0x02040000;	// ���������� ������������ ���� - 4
	I2C2->CR2|=0x00002000;	// ����� ������
	I2C2->TXDR=i2cSendArr[0];
	while ((I2C2->ISR&0x00000001)==0){;}
	I2C2->TXDR=i2cSendArr[1];
	while ((I2C2->ISR&0x00000001)==0){;}
	I2C2->TXDR=i2cSendArr[2];
	while ((I2C2->ISR&0x00000001)==0){;}
	I2C2->TXDR=i2cSendArr[3];
	while ((I2C2->ISR&0x00000001)==0){;}

}

unsigned char GetDataWriteStatus (void)
{
	unsigned char write=0;

	if ((I2C2->ISR&0x00000020)==0x00000020)
	{
		I2C2->ICR|=0x00000020;
		write=1;
	}

	return write;
}

void I2c2ReadData (void)
{
	I2C2->CR2|=0x00000400;	// �������� ����� ������
	I2C2->CR2&=0xFF00FFFF;	// �������� ���-�� ����������� ����
	I2C2->CR2|=0x02020000;  // ���-�� ����������� ���� - 2
	I2C2->CR2|=0x00002000;	// ����� ����������
	while ((I2C2->ISR&0x00000004)==0){;}
	i2cReceiveArr[0]=I2C2->RXDR;
	while ((I2C2->ISR&0x00000004)==0){;}
	i2cReceiveArr[1]=I2C2->RXDR;
}

void AddParam2StoreQueue (unsigned char parNum, unsigned short parVal)
{
	unsigned char k=0,match=0;

	while (k<8)	// ��������� ���� �� ����� �������� � ������� (�� ������)
	{
		if (numQueue[k]==parNum) {match=1; break;} // ���� ����, �� ������ ����
		k++;
	}

	if (match==1)
	{
		valQueue[k]=parVal;	// ���� �������� � �������, �� �������� ��� �������� �� �����
	}
	else	// ����� ���������� ����� ��������
	{
		if (numQueue[q_pointerEnd]==0)	// ���� ����� ��������� �����
		{
			numQueue[q_pointerEnd]=parNum;	// ���������� ����� ��������
			valQueue[q_pointerEnd]=parVal;  // � ��� ��������

			q_pointerEnd++;		// ���������� ��������� ����� �������

			if (q_pointerEnd==8) q_pointerEnd=0; // ������� ����� ������������ ���-�� ����������
		}
	}
}

void SaveParamFromQueue (void)
{
	static unsigned char saving=0;
	static unsigned int counterParamSave=0;

	if (saving==0)	// ���� ������ �� ������������
	{
		counterParamSave++;	// ������� ������� ��� ������ ��������� �� ������� (������ 10 ������)
		if (counterParamSave>=50000)
		{
			if (i2c_busy==0)
			{
				counterParamSave=0;
				if (q_pointerBeg!=q_pointerEnd)
				{
					saving=1;	// ���� ��������� ����� � ������ ������� (� ������� ���� ������), �� ����������.
					i2c_busy=1;
				}
			}
			else counterParamSave=25000;
		}
	}
	else if (saving==1)
	{
		saving=2;
		i2cSendArr[0]=0x00;	// ������� ���� ������
		i2cSendArr[1]=numQueue[q_pointerBeg]*2; // ������� ���� ������
		i2cSendArr[2]=(unsigned char)(valQueue[q_pointerBeg]>>8); // ���������� �������� � ������ ��� ������ ������ �� I2C ������� ����
		i2cSendArr[3]=(unsigned char)(valQueue[q_pointerBeg]);// ���������� ������� ����

		I2c2WriteData();	// ������ ��������� �� I2C
	}
	else if (saving==2)
	{
		if (GetDataWriteStatus()) // ���� ������ �����������
		{
			valQueue[q_pointerBeg]=0; // �� �������� ������� ������� �������
			numQueue[q_pointerBeg]=0; // � ����� ��������� � �������

			q_pointerBeg++;	// ������������� ��������� ������ ������� �� ��������� ��������
			if (q_pointerBeg==8) q_pointerBeg=0; // ������� ����� ������������ ���������� ����������
			saving=0; // ������������ � ������� ������ ���������
			i2c_busy=0;
		}
	}
}

unsigned char ReadParam (unsigned char parNum)
{
	unsigned char readed=0;
	static unsigned char stage=0;

	if (stage==0)	// ���������� ����� � �������� ����� ������ ������
	{
		if (i2c_busy==0)
		{
			i2c_busy=2;
			stage=1;
			i2cSendArr[0]=00;
			i2cSendArr[1]=parNum*2;

			I2c2WriteAddr();
		}
	}
	else if (stage==1)	// ����, ����� ����� ��������� � ������
	{
		if (GetDataWriteStatus()) // ���� ������ �����������
		{
			stage=2;
		}
	}
	else if (stage==2)	// ������ ������ �� ����� ��������� ������
	{
		stage=3;
		I2c2ReadData();
	}
	else if (stage==3)
	{
		if (GetDataWriteStatus()) // ���� ������ �����������
		{
			readed=1;
			stage=0;
			i2c_busy=0;
		}
	}

	return readed;
}

unsigned char ReadParamsFromMem (unsigned char firstParNum, unsigned char numOfParams) // ������ �� ������ ������������������ ����������
{
	unsigned char readed=0,status=0,index=0;
	static unsigned char stage=0, n=0;

	if (stage==0)	// ������������� ��������
	{
		n=0;
		stage=1;
	}
	else if (stage==1)
	{
		if (n!=numOfParams)
		{
			index=firstParNum+n;
			status=ReadParam(index);
			if (status==1)
			{
				SaveParam2Var(index,((unsigned short)i2cReceiveArr[0]<<8)|i2cReceiveArr[1]);// ���������� ���������� ��������� � ����������
				n++;
			}
		}
		else stage=2;
	}
	else if (stage==2)
	{
		readed=1;
		stage=0;
	}

	return readed;
}














