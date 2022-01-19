

void TimerEnable (unsigned char); // ������ ������� - ��������� ���� Counter enable
void TimerClkEnable(unsigned char); // ��������� ������������ �������
void TimerStart (unsigned char); // ������ ���������� �������
void TimerInit (unsigned char); // ������������� ����������� �������
void Timer14Stop (void);
void Val2TimerCCR (unsigned short);

void TIM14_IRQHandler (void);

void TimerParams (unsigned int);

//void TIM6_DAC1_IRQHandler(void);

void SysTickTimerInit(void);
void SysTickTimerLoad (unsigned int);
void SysTickCount (unsigned int);
void SysTickStart(unsigned int);
unsigned char getSysTickStatus (void);

