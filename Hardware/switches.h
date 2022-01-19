
struct switches
{
	unsigned char enc_rght[2];
	unsigned char enc_left[2];
	unsigned char enc_butt[2];

	unsigned char swtch[4];
} spi_switches;

struct button
{
	unsigned short counter;
	unsigned char press;
	unsigned char on;
	unsigned char state;
}panelButtons[4];


unsigned char Encoder1 (void);
unsigned char Encoder2 (void);
void SwitchesData (unsigned short);
unsigned char ButtonPress (unsigned char);
unsigned char TorchSwitch (void);