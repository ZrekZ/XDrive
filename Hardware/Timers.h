

void TimerEnable (unsigned char); // Запуск таймера - установка бита Counter enable
void TimerClkEnable(unsigned char); // Включение тактирования таймера
void TimerStart (unsigned char); // Запуск указанного таймера
void TimerInit (unsigned char); // Инициализация указаннного таймера
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

