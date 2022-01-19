
#include "stm32f303xc.h"
#include "usart.h"
#include "software\menu.h"
#include "dma.h"
#include "crc.h"

#define DRV_UART USART2
#define DRV_TRANSMIT_THR 18 // кол-во посылаемых данных на подающий
#define DRV_RECEIVE_THR 18 // колв-во данных, принимаемых с подающего

unsigned char usart2drvSB[DRV_TRANSMIT_THR];
unsigned char usart2drvRB[DRV_RECEIVE_THR];
extern unsigned short showVoltage, showCurrent;
extern unsigned short trans_temp1, trans_temp2;
extern unsigned char overheat;
extern unsigned char monitor;
extern unsigned char power;


unsigned short r_drossel=0;
unsigned short r_inverterOn=0;
unsigned short r_value=0;

unsigned char ext_button=0;
unsigned char r_monitor=0;
unsigned char r_overheat=0;

unsigned char fb_power=0;
unsigned char drv_transmit=0;
unsigned short r_voltage_ps=0;
unsigned short r_current_ps=0;
unsigned char proc_state = 0;

volatile unsigned char selfreceive=0, receiveStop=2,bCounter=0;
volatile unsigned short timoutCounter=0, self_cnt=0;

//************** Инициализация USART 1 *********************
void Usart1Start(void)
{
	Usart1Clk_enable();
	Usart1_Init();

	USART1->CR1|=0x0000000D;
}

void Usart1_Init (void)
{
	USART1->CR1=0x00000020; // Прерывание, если буфер приема не пуст
	USART1->CR2=0x00000000;
	USART1->CR3=0x00001000; // Overrun disable
	USART1->RTOR=0x00; // 480 бит (5 мсек) ожидания до смены приема на передачу

	USART1->BRR=0x139; // 230 400 бит/с при частоте 72 МГц
}

void Usart1Clk_enable (void)
{
	RCC->APB2ENR|=0x00004000;
	RCC->CFGR3|=0x00000001;
}
//**********************************************************

//************** Инициализация USART 2 *********************
void Usart2Start(void)
{
	Usart2Clk_enable();
	Usart2_Init();

	USART2->CR1|=0x0000000D;
}

void Usart2_Init (void)
{
	USART2->CR1=0x00000020; // Прерывание, если буфер приема не пуст
	USART2->CR2=0x00030000;
	USART2->CR3=0x00001000; // Overrun disable
	USART2->RTOR=0x00; // 480 бит (5 мсек) ожидания до смены приема на передачу

	USART2->BRR=0x5A0; // 230 400 бит/с при частоте 72 МГц
}

void Usart2Clk_enable (void)
{
	RCC->APB1ENR|=0x00020000;
	RCC->CFGR3|=0x00010000;
}
//**********************************************************

//************** Инициализация USART 3 *********************
void Usart3Start(void)
{
	Usart3Clk_enable();
	Usart3_Init();
	USART3->CR1|=0x0000000D;
}

void Usart3_Init (void)
{
	USART3->CR1=0x00;
	USART3->CR2=0x00800000;
	USART3->CR3|=0x00001000;
	USART3->RTOR=0x32;

	USART3->BRR=0x78; // 600 000 бит/с при частоте 72 МГц
}

void Usart3Clk_enable (void)
{
	RCC->APB1ENR|=0x00040000;
	RCC->CFGR3|=0x00040000;
}
//**********************************************************

void USART2_IRQHandler(void) // Прерывание слейва
{
	unsigned char data;
	static unsigned char start_cnt=0;

	data=DRV_UART->RDR;
    self_cnt=0;
	timoutCounter=0;
	if (selfreceive==0)
	{
		selfreceive=1;
		if (data==0xFF)
	   {
	   		start_cnt++;
			if (start_cnt==3) {bCounter=0;DRV_UART->TDR=0x55;}
	   }
		else {start_cnt=0;}

		if (start_cnt!=3)
		{
	   		receiveStop=0;
			usart2drvRB[bCounter]=data;
			DRV_UART->TDR=usart2drvSB[bCounter];
			bCounter++;

		}
		if (start_cnt==3) start_cnt=0;
	}
	else
	{
		selfreceive=0;
		timoutCounter=0;
		if (bCounter==DRV_RECEIVE_THR)
		{
			//if ((usart2drvRB[6]^usart2drvRB[7])==(usart2drvRB[8]))
			//{
			//	 if ((usart2drvRB[6]&0xC0)==0xC0)
			//	 {
			//		fb_power=(usart2drvRB[6]>>2)&0x01;
			//	 }
			//}
			
			bCounter=0;
			receiveStop=1;
			DRV_UART->RQR|=0x08;
		}
	}

}

void ExtractDRVData (void)
{
	volatile static unsigned short temp = 0;
	
	if (receiveStop==1)  // если идет прием данных, а принятые данные еще никто не разбирал
	{
		CRCReset();
		temp = CRC_Calculate(&usart2drvRB[0],0,10);
		
		if (temp == ((unsigned short)usart2drvRB[10]<<8)|usart2drvRB[11])
		{
			if ((usart2drvRB[0]^usart2drvRB[1])==(usart2drvRB[2]))
			{
				 r_voltage_ps=((unsigned short)usart2drvRB[1]<<8)|usart2drvRB[0];
				 usart2drvRB[2]^=0xAA;
			}
			
			if ((usart2drvRB[3]^usart2drvRB[4])==(usart2drvRB[5]))
			{
				 r_current_ps=((unsigned short)usart2drvRB[4]<<8)|usart2drvRB[3];
				 usart2drvRB[5]^=0xAA;
			}
			
			if ((usart2drvRB[6]&0xC0)==0xC0)
			{
				
			  fb_power=(usart2drvRB[6]>>2)&0x01;
				r_overheat=usart2drvRB[6]&0x01;
			  r_monitor=(usart2drvRB[6]>>1)&0x01;
			  //fb_power=(usart2drvRB[6]>>2)&0x01;
			  proc_state = (usart2drvRB[6]>>4)&0x03;
			}
		}

		receiveStop=2;
	}
}

void PackDRVData (void)
{
	unsigned short temp; 
	if (receiveStop==2)
	{
		
		if ((send_voltage_flag == 1)||(send_voltage_flag == 2))
		{
			usart2drvSB[0]=(unsigned char)((unsigned short)send_voltage);
			usart2drvSB[1]=(unsigned char)(((unsigned short)send_voltage)>>8);
		}
		else
		{
			usart2drvSB[0]=(unsigned char)(userParameters.voltage);
			usart2drvSB[1]=(unsigned char)(userParameters.voltage>>8);
		}
		
		//if (usart2drvSB[1]==0xFF) usart2drvSB[1]=0xFE;
		usart2drvSB[2]=(usart2drvSB[0]^usart2drvSB[1]);
		usart2drvSB[3]=(unsigned char)(addParams.pulsTime);
		usart2drvSB[4]=(unsigned char)(addParams.pulsTime>>8);
		usart2drvSB[5]=(usart2drvSB[3]^usart2drvSB[4]);
		
		if ((send_voltage_flag == 1)&&(zkmode == 0)&&(addParams.pulsMode > 0))
		{
			usart2drvSB[6]=(unsigned char)(drossel2);
			usart2drvSB[7]=(unsigned char)(drossel2>>8);
		}
		else
		{
			usart2drvSB[6]=(unsigned char)(userParameters.drossel);
			usart2drvSB[7]=(unsigned char)(userParameters.drossel>>8);
		}
		
		usart2drvSB[8]=(usart2drvSB[6]^usart2drvSB[7]);
		usart2drvSB[9]=power;
		if ((send_voltage_flag == 1)&&(zkmode == 0))
		{
			usart2drvSB[9]|=(((unsigned char)0)<<1);
		}
		else
		{
			usart2drvSB[9]|=(((unsigned char)addParams.pulsMode)<<1);
		}
		usart2drvSB[9]|=0x04;
		usart2drvSB[9]|=0xC0;
		usart2drvSB[10]=(0x5A);
		usart2drvSB[11]=(usart2drvSB[9]^usart2drvSB[10]);
		usart2drvSB[12]=(unsigned char)(addParams.pulsCurrent);
		usart2drvSB[13]=(unsigned char)(addParams.pulsCurrent>>8);
		//usart2drvSB[14]=(usart2drvSB[12]^usart2drvSB[13]);
		usart2drvSB[14]=(unsigned char)(addParams.backCurrent);
		usart2drvSB[15]=(unsigned char)(addParams.backCurrent>>8);
		
		CRCReset();
		temp = CRC_Calculate(&usart2drvSB[0],0,16);
		
		usart2drvSB[16]=(unsigned char)(temp>>8);
		usart2drvSB[17]= (unsigned char)temp;
		
		//usart2drvSB[17]=(usart2drvSB[15]^usart2drvSB[16]);

		receiveStop=3;
	}
}

void DrvTransmitControl (void)
{
	if (receiveStop==3)
	{
		receiveStop=4;
		bCounter=0;
		selfreceive=0;
	}

	if (selfreceive==1)
	{
		self_cnt++;
		if (self_cnt>=5000)
		{
			selfreceive=0;
			bCounter=0;
			self_cnt=0;
		}
	}
	else self_cnt=0;
}


/*
void usart2 (void)
{
	static unsigned char counter=0;

	if ((USART2->ISR&0x00000020)==0x00000020) // Флаг о том, что есть непрочитанные данные
	{
		usart2ReceiveReg[counter]=USART2->RDR; // прием данных в буфер
		counter++;
	}
	else if ((USART2->ISR&0x00000800)==0x00000800) // Флаг таймаута по времени
	{

		USART2->ICR|=0x00000800; // Сброс флага таймаута
		USART2->RQR|=0x08; // Сброс флага о принятых данных
		DataParsing(); // Разбор принятых данных
		counter=0; // Сброс счетчика принятых данных
	}
}
*/

/*
void SendData (void)
{
	PrepareData(); 				// Подготовка данных к отправке
	setDma1DataWidth (7,39);	// установка кол-ва пересылаемых данных для канала 7
	Dma1ChannelOnOffState(7,1); // начало передачи в буфер отправки по DMA
}
*/

/*
void DataParsing (void)
{
	if ((usart2ReceiveReg[0]==0xFF)&&(usart2ReceiveReg[1]==0xFF)&&(usart2ReceiveReg[2]==0xFF))
	{
		if ((usart2ReceiveReg[3]^usart2ReceiveReg[4])==usart2ReceiveReg[5])
		{
			r_inverterOn=((usart2ReceiveReg[3]&0x80)>>7);
			r_drossel=((usart2ReceiveReg[3]&0x40)>>6);
			r_value= (((unsigned int)usart2ReceiveReg[3]&0x3F)<<8)|(usart2ReceiveReg[4]);

			changeVoltage(r_value);
			changeDrossel(r_drossel);
			//power_on=r_inverterOn;

			ext_button=r_inverterOn;

			usart2ReceiveReg[5]|=0xAA;
		}

		usart2ReceiveReg[0]=0xAA; usart2ReceiveReg[1]=0xAA; usart2ReceiveReg[2]=0xAA;
	}
}
*/





