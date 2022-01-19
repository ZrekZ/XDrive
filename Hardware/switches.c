#include "stm32f303xc.h"
#include "switches.h"

// Правый энкодер
#define ENC_RGHT_1	0x0200
#define ENC_LEFT_1  0x0800
#define ENC_SWTCH_1 0x0100

// Левый энкодер
#define ENC_RGHT_2  0x0010
#define ENC_LEFT_2  0x0020
#define ENC_SWTCH_2 0x0080

// Кнопки
#define SWITCH_1 0x1000
#define SWITCH_2 0x2000
#define SWITCH_3 0x0002
#define SWITCH_4 0x0004

// Кнопка горелки
#define TSWITCH ((GPIOA->IDR&0x00000800)==0)


unsigned char Encoder1 (void)
{
	unsigned char encoder=0, enc_move=0;
	static unsigned char enc_count=0, enc_count_=0,enc=0,enc_=0;
	static unsigned short enc_timer_=0;
	static unsigned int press_counter=0;

	encoder=0;

	if (spi_switches.enc_rght[0]) encoder|=0x08;
	if (spi_switches.enc_left[0]) encoder|=0x10;

	if (spi_switches.enc_butt[0]==0)
	{
		if ((enc_count==0)&&(encoder==0)) {enc_count=1;enc_count_=2;}
		else if ((enc_count==1)&&(encoder==16)) {enc_count=2;enc_count_=1;}
		else if ((enc_count==2)&&(encoder==24)) {enc_count=3;enc_count_=0;}
		else if ((enc_count==3)&&(encoder==8))
		{
			enc_count=0;enc_move=2; enc_count_=0;
		}

		if ((enc_count_==0)&&(encoder==16)) {enc_count_=1;enc_count=2;}
		else if ((enc_count_==1)&&(encoder==0)) {enc_count_=2;enc_count=1;}
		else if ((enc_count_==2)&&(encoder==8)) {enc_count_=3;enc_count=0;}
		else if  ((enc_count_==3)&&(encoder==24))
		{
			enc_count_=0;enc_move=1; enc_count=0;
		}

		{enc_timer_=0;enc=0;}
	}
	else
	{
		enc_timer_++;
		if (enc_timer_==150)
		{
			enc_timer_=0;
			enc=4;
		}
	}

	if (enc==4)
	{
		if (enc_==0){enc_=1;}
		if (enc_==2){enc_=3;}

		if ((enc_==1)||(enc_==3))
		{
			press_counter++;
			if (press_counter>=40000)
			{
				enc_move=3;
				press_counter=40000;
			}
		}
	}
	else if (enc==0)
	{
		press_counter=0;

		if (enc_==1)
		{
			enc_=2;
			enc_move=4;
			enc_count=0;
			enc_count_=0;
		}
		if (enc_==3)
		{
			enc_=0;
			enc_move=4;
			enc_count=0;
			enc_count_=0;
		}
	}

	return enc_move;
}

unsigned char  Encoder2 (void)
{
	unsigned char encoder=0,enc_move=0;
	static unsigned char enc_count=0, enc_count_=0,enc=0, enc_=0;
	static unsigned short enc_timer_=0;
	static unsigned int press_counter=0;

	encoder=0;

	if (spi_switches.enc_rght[1]) encoder|=0x08;
	if (spi_switches.enc_left[1]) encoder|=0x10;

	if (spi_switches.enc_butt[1]==0)
	{
		if ((enc_count==0)&&(encoder==0)) {enc_count=1;enc_count_=2;}
		else if ((enc_count==1)&&(encoder==16)) {enc_count=2;enc_count_=1;}
		else if ((enc_count==2)&&(encoder==24)) {enc_count=3;enc_count_=0;}
		else if ((enc_count==3)&&(encoder==8))
		{
			enc_count=0;enc_move=2; enc_count_=0;
		}

		if ((enc_count_==0)&&(encoder==16)) {enc_count_=1;enc_count=2;}
		else if ((enc_count_==1)&&(encoder==0)) {enc_count_=2;enc_count=1;}
		else if ((enc_count_==2)&&(encoder==8)) {enc_count_=3;enc_count=0;}
		else if  ((enc_count_==3)&&(encoder==24))
		{
			enc_count_=0;enc_move=1; enc_count=0;
		}

		{enc_timer_=0;enc=0;}
	}
	else
	{
		enc_timer_++;
		if (enc_timer_==150)
		{
			enc_timer_=0;
			enc=4;
		}
	}

	if (enc==4)
	{
		if (enc_==0){enc_=1;}
		if (enc_==2){enc_=3;}

		if ((enc_==1)||(enc_==3))
		{
			press_counter++;
			if (press_counter>=40000)
			{
				enc_move=3;
				press_counter=40000;
			}
		}
	}
	else if (enc==0)
	{
		press_counter=0;

		if (enc_==1)
		{
			enc_=2;
			enc_move=4;
			enc_count=0;
			enc_count_=0;
		}
		if (enc_==3)
		{
			enc_=0;
			enc_move=4;
			enc_count=0;
			enc_count_=0;
		}
	}

	return enc_move;
}

void SwitchesData (unsigned short in_data) // Разборка данных, полученных по SPI от переключателей
{
	if ((in_data&ENC_RGHT_1)==0) spi_switches.enc_rght[0]=1;
	else spi_switches.enc_rght[0]=0;
	if ((in_data&ENC_LEFT_1)==0) spi_switches.enc_left[0]=1;
	else spi_switches.enc_left[0]=0;
	if ((in_data&ENC_SWTCH_1)==0) spi_switches.enc_butt[0]=1;
	else spi_switches.enc_butt[0]=0;
	if ((in_data&ENC_RGHT_2)==0) spi_switches.enc_rght[1]=1;
	else spi_switches.enc_rght[1]=0;
	if ((in_data&ENC_LEFT_2)==0) spi_switches.enc_left[1]=1;
	else spi_switches.enc_left[1]=0;
	if ((in_data&ENC_SWTCH_2)==0) spi_switches.enc_butt[1]=1;
	else spi_switches.enc_butt[1]=0;
	if ((in_data&SWITCH_1)==0) spi_switches.swtch[0]=1;
	else spi_switches.swtch[0]=0;
	if ((in_data&SWITCH_2)==0) spi_switches.swtch[1]=1;
	else spi_switches.swtch[1]=0;
	if ((in_data&SWITCH_3)==0) spi_switches.swtch[2]=1;
	else spi_switches.swtch[2]=0;
	if ((in_data&SWITCH_4)==0) spi_switches.swtch[3]=1;
	else spi_switches.swtch[3]=0;
}

unsigned char ButtonPress (unsigned char switchNum)
{
	// Обработчик кнопки с антидребезгом
	if (spi_switches.swtch[switchNum])			// Если кнопка нажата
	{
      panelButtons[switchNum].counter++;        // Ожидаем некоторое время
	  if (panelButtons[switchNum].counter>=300)
	  {
	  		panelButtons[switchNum].press=1;      // Кнопка нажата
			panelButtons[switchNum].counter=300;
	  }
	}
	else if (panelButtons[switchNum].counter>0)		// Если кнопку отпустили, то антидребезг на отпускание
	{
		panelButtons[switchNum].counter--;
	}
	else {panelButtons[switchNum].counter=0;panelButtons[switchNum].press=0;} // Кнопка не нажата


	// Данный кусок используется для однократного срабатывания кнопки при удержании
	if (panelButtons[switchNum].press==1)
	{
		if (panelButtons[switchNum].on==0) {panelButtons[switchNum].on=1;panelButtons[switchNum].state=1;}
		else panelButtons[switchNum].state=0;
	}
	else if (panelButtons[switchNum].press==0)
	{
		if (panelButtons[switchNum].on==1){panelButtons[switchNum].on=0;panelButtons[switchNum].state=0;}
	}

	return  panelButtons[switchNum].state;
}

unsigned char TorchSwitch (void)
{
	static unsigned short tSwitchCounter=0;
	static unsigned char state=0,on=0;
	static unsigned char tSwitchPress=0;
	unsigned char temp=0;

    temp=TSWITCH;

	if (temp)
	{
		tSwitchCounter++;
		if (tSwitchCounter>=1000)
		{
			tSwitchCounter=1000;
			tSwitchPress=1;
		}
	}
	else if (tSwitchCounter>0)
	{
	    tSwitchCounter--;
	}
	else
	{
		tSwitchCounter=0;
		tSwitchPress=0;
	}

	if (tSwitchPress)
	{
		if (state==0) {state=1;on=1;}
		else if (state==2) {state=3;on=3;}
	}
	else if (~tSwitchPress)
	{
		if (state==1) {state=2;on=2;}
		else if (state==3) {state=0;on=0;}
	}

	return on;
}



