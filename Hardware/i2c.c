#include "stm32f303xc.h"
#include "i2c.h"
#include "dma.h"
#include "software/menu.h"

unsigned char i2cReceiveArr[2]={0}, i2cSendArr[4]={0};

unsigned char numQueue[8]={0};
unsigned short valQueue[8]={0};
unsigned char q_pointerEnd=0, q_pointerBeg=0;
unsigned char i2c_busy=0;

void I2c2Clk_enable (void)	// Тактирование I2C1
{
	// Тактирование I2C1
	//RCC->CFGR3|=0x00000010;	// тактирование от SYSCLK
	//RCC->APB1ENR|=0x00200000;

	// Тактирование I2C2
	RCC->CFGR3|=0x00000020;	// тактирование от SYSCLK
	RCC->APB1ENR|=0x00400000;
}

void I2c2Init (void)	// Инициализация регистров
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

void I2C2Start (void)	// Запуск I2C
{
	I2c2Clk_enable();
	I2c2Init();
	I2c2_enable();
}

void I2c2WriteAddr (void)
{

	I2C2->CR2&=0xFFFFFBFF;	// Включаем режим записи
	I2C2->CR2&=0xFF00FFFF;	// Обнуляем кол-во считываемых байт
	I2C2->CR2|=0x02020000;	// Количество записываемых байт - 2
	I2C2->CR2|=0x00002000;	// Старт записи
	I2C2->TXDR=i2cSendArr[0];
	while ((I2C2->ISR&0x00000001)==0){;}
	I2C2->TXDR=i2cSendArr[1];
	while ((I2C2->ISR&0x00000001)==0){;}
}

void I2c2WriteData (void)
{
	I2C2->CR2&=0xFFFFFBFF;	// Включаем режим записи
	I2C2->CR2&=0xFF00FFFF;	// Обнуляем кол-во считываемых байт
	I2C2->CR2|=0x02040000;	// Количество записываемых байт - 4
	I2C2->CR2|=0x00002000;	// Старт записи
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
	I2C2->CR2|=0x00000400;	// Влкючаем режим чтения
	I2C2->CR2&=0xFF00FFFF;	// Обнуляем кол-во считываемых байт
	I2C2->CR2|=0x02020000;  // Кол-во считываемых байт - 2
	I2C2->CR2|=0x00002000;	// Старт считывания
	while ((I2C2->ISR&0x00000004)==0){;}
	i2cReceiveArr[0]=I2C2->RXDR;
	while ((I2C2->ISR&0x00000004)==0){;}
	i2cReceiveArr[1]=I2C2->RXDR;
}

void AddParam2StoreQueue (unsigned char parNum, unsigned short parVal)
{
	unsigned char k=0,match=0;

	while (k<8)	// Проверяем есть ли такой параметр в очереди (по номеру)
	{
		if (numQueue[k]==parNum) {match=1; break;} // Если есть, то ставим флаг
		k++;
	}

	if (match==1)
	{
		valQueue[k]=parVal;	// Если параметр в очереди, то изменяем его значение на новое
	}
	else	// Иначе записываем новый параметр
	{
		if (numQueue[q_pointerEnd]==0)	// Если место параметра пусто
		{
			numQueue[q_pointerEnd]=parNum;	// Записываем номер парметра
			valQueue[q_pointerEnd]=parVal;  // и его значение

			q_pointerEnd++;		// Прибавляем указатель конца очереди

			if (q_pointerEnd==8) q_pointerEnd=0; // Переход через максимальное кол-во параметров
		}
	}
}

void SaveParamFromQueue (void)
{
	static unsigned char saving=0;
	static unsigned int counterParamSave=0;

	if (saving==0)	// Если запись не производится
	{
		counterParamSave++;	// счетчик времени для записи параметра по времени (каждые 10 секунд)
		if (counterParamSave>=50000)
		{
			if (i2c_busy==0)
			{
				counterParamSave=0;
				if (q_pointerBeg!=q_pointerEnd)
				{
					saving=1;	// Если указатель конца и начала неравны (в очереди есть данные), то записываем.
					i2c_busy=1;
				}
			}
			else counterParamSave=25000;
		}
	}
	else if (saving==1)
	{
		saving=2;
		i2cSendArr[0]=0x00;	// Старший байт адреса
		i2cSendArr[1]=numQueue[q_pointerBeg]*2; // Младший байт адреса
		i2cSendArr[2]=(unsigned char)(valQueue[q_pointerBeg]>>8); // Записываем значение в массив для записи данных по I2C старший байт
		i2cSendArr[3]=(unsigned char)(valQueue[q_pointerBeg]);// Записываем младший байт

		I2c2WriteData();	// Запись параметра по I2C
	}
	else if (saving==2)
	{
		if (GetDataWriteStatus()) // Если запись закончилась
		{
			valQueue[q_pointerBeg]=0; // то обнуляем текущую позицию очереди
			numQueue[q_pointerBeg]=0; // и номер параметра в очереди

			q_pointerBeg++;	// Устанавливаем указатель начала очереди на следующее значение
			if (q_pointerBeg==8) q_pointerBeg=0; // Переход через максимальное количество параметров
			saving=0; // Возвращаемся к таймеру записи параметра
			i2c_busy=0;
		}
	}
}

unsigned char ReadParam (unsigned char parNum)
{
	unsigned char readed=0;
	static unsigned char stage=0;

	if (stage==0)	// Записываем адрес с которого будем читать данные
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
	else if (stage==1)	// Ждем, когда адрес запишется в память
	{
		if (GetDataWriteStatus()) // Если запись закончилась
		{
			stage=2;
		}
	}
	else if (stage==2)	// Читаем данные по ранее заданному адресу
	{
		stage=3;
		I2c2ReadData();
	}
	else if (stage==3)
	{
		if (GetDataWriteStatus()) // Если запись закончилась
		{
			readed=1;
			stage=0;
			i2c_busy=0;
		}
	}

	return readed;
}

unsigned char ReadParamsFromMem (unsigned char firstParNum, unsigned char numOfParams) // Чтение из памяти последовательности параметров
{
	unsigned char readed=0,status=0,index=0;
	static unsigned char stage=0, n=0;

	if (stage==0)	// Инициализация передачи
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
				SaveParam2Var(index,((unsigned short)i2cReceiveArr[0]<<8)|i2cReceiveArr[1]);// Сохранение считанного параметра в переменную
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














