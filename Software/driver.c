
unsigned char DriverProcess (unsigned char turn, unsigned short ref)
{
	static unsigned short pwm;
	static unsigned int rev_cntr=0;
	unsigned char mode;
	signed short refVal;

	static unsigned short count=0;

	if (turn == 0) // торможение
	{
		{mode=0; refVal=0;}
	}
	else if (turn == 1) // прямой ход
	{
		{mode=1; refVal=ref;}
		step=0;
		rev_cntr=0;
	}
	else if (turn == 2) // реверс двигателя
	{
		if (step == 0)
		{
			{mode=0;refVal=0;}
			if (pwm == 0) {SetRelay(1);step=1;} // переключение реле на обратный ход
		}
		else if (step == 1) 
		{
			{mode=1; refVal=5;}
			rev_cntr++;
			if (rev_cntr>=4000)
			{
				step = 2;
			}
		}
		else 
		{ 
			{mode=0;refVal=0;}
			if (pwm == 0) { SetRelay(0); turn = 0; } // переключение реле на прямой ход
		}
	}
	
	pwm=DriverRegulator(mode, refVal);
	Val2TimerCCR(pwm);
}

unsigned short CurrentPiRegulator ( unsigned char mode, signed short ref )
{
	static 	float currentError=0;
			float lastCurrentError;
			float currentRef;
			float currentProp;
			float currentDiff;
			float piRegOut;
	static  float ref_use, Vout;
	static 	unsigned int exitCurrReg;
	static float use_adcCurrent_f;
	

		if (adcReceiveArr[2]>adcReceiveArr[1]) use_adcCurrent=-(float)adcReceiveArr[2]*realki;
		else use_adcCurrent=(float)adcReceiveArr[1]*realki;

		realCurrent=(1023*realCurrent+use_adcCurrent)/1024;

		if (level>=100) currentRef=-currentPiReg.ref;
		else currentRef=currentPiReg.ref; 				// Опорное значение для ПИД-регулятора
		lastCurrentError=currentError; 					// Предыдущая ошибка для дифференцирующей составляющей
		currentError=currentRef-use_adcCurrent;			// Ошибка регулирования
		currentInt=currentInt+currentPiReg.intgr1*currentError; // Вычисдение интегральной составляющей
		if (currentInt>4095.0f) currentInt=4095.0f; 	// Ограничение интегрирующей части в максимум
		if (currentInt<-4095.0f) currentInt=-4095.0f;	// Ограничение интегрирующей части в минимум
		currentDiff=(currentError-lastCurrentError)*currentPiReg.diff; // Вычисление дифференцирующей составляющей
		currentProp=currentPiReg.prop1*currentError; 	// Вычисление пропорциональной составляющей
		piRegOut=currentInt+currentProp+currentDiff; 	// Выход ПИД-регулятора
        exitCurrReg=(unsigned int)piRegOut;
		if (exitCurrReg>4091) exitCurrReg=4091;
		
		//***************************************
		//********* НАПРЯжЕНИЕ
		//***************************************
		if (mode == 1)
		{
			use_adcCurrent_f=currentPiReg.maxVal*(use_adcCurrent-use_adcCurrent_f)+use_adcCurrent_f;
		 // realVoltage=realVoltage+voltagePiReg.maxVal*((float)adcReceiveArr[0]-realVoltage); // фильтр 1-го порядка на напряжение
		  //use_adcVoltage=(20.0f*((float)realVoltage-kuout)-1.0f*((float)realVoltage-realku))/(realku-kuout); // Приведение напряжения АЦП к вольтам
		  ref_use=((voltagePiReg.prop2-1.0f)*ref_use + ref)/voltagePiReg.prop2; // Ограничение скорости измененния уставки фильтром																				 // voltagePiReg.prop2 - коэффициент фильтра
		  Vout=((ref_use+use_adcCurrent_f*res+voltagePiReg.intgr2)*voltagePiReg.prop1)/(float)adcReceiveArr[3]; //
		  if (Vout>4090) Vout=4090;
		  //Vout=4090;
		}
		
		if (Vout<exitCurrReg) 
		{
			currentInt=Vout;
			exitCurrReg=Vout;
		}

  	return (unsigned short)exitCurrReg;
}