

void setMotorOn (unsigned char);		// ���������/���������� ���������
unsigned char getSwitchStatus (unsigned char);	// ��������� ������, � ������ -
												// ������ ������� ������� ���������
												// � ����������� �� ���������� ������ (2�, 4T, ��������, ������������)
void setValveOn (unsigned char);		// ���������/���������� �������

unsigned char getMonitorStatus (void);	// ��������� �������� �������
void setFan (unsigned char);		 	// ���������/���������� �����������
void setSoftStart (unsigned char);		// ������� ����
void setCoolingOn (unsigned char);		// ���������/���������� ����������
unsigned char getFlowStatus (void);		// ��������� ������� ������
void SetValve1 (unsigned char );
void SetValve2 (unsigned char );
void SetRelay (unsigned char );


//void clk2mco (unsigned char); // ����� ������� ������������ �� ����� MCO (PA8)
