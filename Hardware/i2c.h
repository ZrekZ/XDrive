
void I2c2Clk_enable (void); // Тактирование I2C2
void I2c2Init (void);	// Инициализация регистров
void I2c2_enable (void);
void I2C2Start (void);	// Запуск I2C
void I2c2WriteAddr (void); // Установка адреса считывания в памяти
void I2c2WriteData (void); // Запись данных в память
unsigned char GetDataWriteStatus (void); // Проверка, записались ли данные
void I2c2ReadData (void);  // Чтение данных из памяти по ранее установленному адресу
void AddParam2StoreQueue (unsigned char, unsigned short);
void SaveParamFromQueue (void); // Сохранение следующего в очереди параметра
unsigned char ReadParam (unsigned char); // Считывание одного параметра
unsigned char ReadParamsFromMem (unsigned char firstParNum, unsigned char numOfParams); // Считывание серии параметров