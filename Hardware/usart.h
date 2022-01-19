

void Usart1_Init (void);
void Usart1Clk_enable (void);
void Usart1Start(void);
void Usart2Clk_enable (void);
void Usart2_Init (void);
void Usart2Start(void);
void Usart3Start(void);
void Usart3_Init (void);
void Usart3Clk_enable (void);
void SendData (void);
void DataParsing (void);
void PrepareData (void);
void usart2 (void);
void UsartSendData2Drive (void);
void ExtractDRVData (void);
void PackDRVData (void);
void USART2_IRQHandler(void);
void DrvTransmitControl (void);