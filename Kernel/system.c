#include "system.h"
#include "interrupts.h"
#include "utility.h"

struct idtEntry idt[256];
struct idtPtr idtp;





void STI()
{
	__asm__("sti");
}

void CLI()
{
	__asm__("cli");
}


void IDTSetGate(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags)
{
	idt[num].base_low = (short)base;
	idt[num].base_high = (short)(base >> 16);
	idt[num].selector = selector;
	idt[num].flags = flags;
	idt[num].zero = 0;
}

void IDTInstall()
{
	idtp.limit = (sizeof (struct idtEntry) * 256) - 1;
    idtp.base = &idt;
	memset(&idt, 0, 0x800);
	__asm__("lidt (%%ebx)"::"b"(&idtp));
	ISRInstall();
}

