
void Dma1Start (void);
void Dma1_Init (void);
void Dma1Clk_enable (void);
void setDma1DataWidth(unsigned char,unsigned int);
void set_Dma1MemPointer_short (unsigned char, unsigned short*);
void set_Dma1MemPointer_char (unsigned char, unsigned char*);
void set_Dma1PeriphPointer_uint16t (unsigned char, uint16_t volatile*);
void set_Dma1PeriphPointer_uint32t (unsigned char, uint32_t volatile*);
void Dma1ChannelOnOffState (unsigned char,unsigned char);

void DMA1_Channel1_IRQHandler(void);