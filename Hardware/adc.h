
void AdcStart (void);
void AdcClk_enable (void);
void AdcCalibrate (void);
void AdcInit (void);
void AdcEnable (void);
void ADCStart(unsigned char);
void SetADCRegSequence (unsigned char , unsigned char );
void SetADCRegSequenceLength (unsigned char );
void SetADCInjSequenceLength (unsigned char );
void SetADCInjSequence (unsigned char , unsigned char );
void ReadInjADC (void);
void OPAMP_Init(unsigned char num);
void ADC1_2_IRQHandler(void);

struct adcVal
{
	unsigned short temp1;
	unsigned short temp2;
	unsigned short curref;
};

struct adcVal injADC;
struct adcVal filter;
struct adcVal showVal;