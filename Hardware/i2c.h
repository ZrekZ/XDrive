
void I2c2Clk_enable (void); // ������������ I2C2
void I2c2Init (void);	// ������������� ���������
void I2c2_enable (void);
void I2C2Start (void);	// ������ I2C
void I2c2WriteAddr (void); // ��������� ������ ���������� � ������
void I2c2WriteData (void); // ������ ������ � ������
unsigned char GetDataWriteStatus (void); // ��������, ���������� �� ������
void I2c2ReadData (void);  // ������ ������ �� ������ �� ����� �������������� ������
void AddParam2StoreQueue (unsigned char, unsigned short);
void SaveParamFromQueue (void); // ���������� ���������� � ������� ���������
unsigned char ReadParam (unsigned char); // ���������� ������ ���������
unsigned char ReadParamsFromMem (unsigned char firstParNum, unsigned char numOfParams); // ���������� ����� ����������