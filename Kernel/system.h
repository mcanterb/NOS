

struct idtEntry
{
	unsigned short base_low;
	unsigned short selector;
	unsigned char zero;
	unsigned char flags;
	unsigned short base_high;
} __attribute__((packed));

struct idtPtr
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

volatile extern void IDTLoad();
void STI();
void CLI();
void IDTSetGate(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags);
void IDTInstall();