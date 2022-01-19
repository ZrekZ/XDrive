

void setMotorOn (unsigned char);		// Влкючение/отключение двигателя
unsigned char getSwitchStatus (unsigned char);	// Состояние кнопки, а точнее -
												// выдача сигнала запуска двигателя
												// в зависимости от выбранного режима (2Т, 4T, Точечный, Интервальный)
void setValveOn (unsigned char);		// Включение/выключение клапана

unsigned char getMonitorStatus (void);	// Состояние монитора питания
void setFan (unsigned char);		 	// Включение/отключение вентилятора
void setSoftStart (unsigned char);		// Плавный пуск
void setCoolingOn (unsigned char);		// Включение/отключение охлаждения
unsigned char getFlowStatus (void);		// Состояние датчика потока
void SetValve1 (unsigned char );
void SetValve2 (unsigned char );
void SetRelay (unsigned char );


//void clk2mco (unsigned char); // Вывод сигнала тактирования на ножку MCO (PA8)
