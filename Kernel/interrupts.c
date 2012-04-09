#include "system.h"
#include "interrupts.h"
#include "video.h"
#include "utility.h"



IRQRoutine handlers[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


void ISRInstall()
{
	IDTSetGate(0, (unsigned long)&isr0, 0x08, 0x8E);
	IDTSetGate(1, (unsigned long)&isr1, 0x08, 0x8E);
	IDTSetGate(2, (unsigned long)&isr2, 0x08, 0x8E);
	IDTSetGate(3, (unsigned long)&isr3, 0x08, 0x8E);
	IDTSetGate(4, (unsigned long)&isr4, 0x08, 0x8E);
	IDTSetGate(5, (unsigned long)&isr5, 0x08, 0x8E);
	IDTSetGate(6, (unsigned long)&isr6, 0x08, 0x8E);
	IDTSetGate(7, (unsigned long)&isr7, 0x08, 0x8E);
	IDTSetGate(8, (unsigned long)&isr8, 0x08, 0x8E);
	IDTSetGate(9, (unsigned long)&isr9, 0x08, 0x8E);
	IDTSetGate(10, (unsigned long)&isr10, 0x08, 0x8E);
	IDTSetGate(11, (unsigned long)&isr11, 0x08, 0x8E);
	IDTSetGate(12, (unsigned long)&isr12, 0x08, 0x8E);
	IDTSetGate(13, (unsigned long)&isr13, 0x08, 0x8E);
	IDTSetGate(14, (unsigned long)&isr14, 0x08, 0x8E);
	IDTSetGate(15, (unsigned long)&isr15, 0x08, 0x8E);
	IDTSetGate(16, (unsigned long)&isr16, 0x08, 0x8E);
	IDTSetGate(17, (unsigned long)&isr17, 0x08, 0x8E);
	IDTSetGate(18, (unsigned long)&isr18, 0x08, 0x8E);
	IDTSetGate(19, (unsigned long)&isr19, 0x08, 0x8E);
	IDTSetGate(20, (unsigned long)&isr20, 0x08, 0x8E);
	IDTSetGate(21, (unsigned long)&isr21, 0x08, 0x8E);
	IDTSetGate(22, (unsigned long)&isr22, 0x08, 0x8E);
	IDTSetGate(23, (unsigned long)&isr23, 0x08, 0x8E);
	IDTSetGate(24, (unsigned long)&isr24, 0x08, 0x8E);
	IDTSetGate(25, (unsigned long)&isr25, 0x08, 0x8E);
	IDTSetGate(26, (unsigned long)&isr26, 0x08, 0x8E);
	IDTSetGate(27, (unsigned long)&isr27, 0x08, 0x8E);
	IDTSetGate(28, (unsigned long)&isr28, 0x08, 0x8E);
	IDTSetGate(29, (unsigned long)&isr29, 0x08, 0x8E);
	IDTSetGate(30, (unsigned long)&isr30, 0x08, 0x8E);
	IDTSetGate(31, (unsigned long)&isr31, 0x08, 0x8E);
}


void IRQRemap(void)
{
    out(0x20, 0x11);
    out(0xA0, 0x11);
    out(0x21, 0x20);
    out(0xA1, 0x28);
    out(0x21, 0x04);
    out(0xA1, 0x02);
    out(0x21, 0x01);
    out(0xA1, 0x01);
    out(0x21, 0x0);
    out(0xA1, 0x0);
}
void IRQInstall()
{
	IRQRemap();
	IDTSetGate(32, (unsigned long)&irq0, 0x08, 0x8E);
	IDTSetGate(33, (unsigned long)&irq1, 0x08, 0x8E);
	IDTSetGate(34, (unsigned long)&irq2, 0x08, 0x8E);
	IDTSetGate(35, (unsigned long)&irq3, 0x08, 0x8E);
	IDTSetGate(36, (unsigned long)&irq4, 0x08, 0x8E);
	IDTSetGate(37, (unsigned long)&irq5, 0x08, 0x8E);
	IDTSetGate(38, (unsigned long)&irq6, 0x08, 0x8E);
	IDTSetGate(39, (unsigned long)&irq7, 0x08, 0x8E);
	IDTSetGate(40, (unsigned long)&irq8, 0x08, 0x8E);
	IDTSetGate(41, (unsigned long)&irq9, 0x08, 0x8E);
	IDTSetGate(42, (unsigned long)&irq10, 0x08, 0x8E);
	IDTSetGate(43, (unsigned long)&irq11, 0x08, 0x8E);
	IDTSetGate(44, (unsigned long)&irq12, 0x08, 0x8E);
	IDTSetGate(45, (unsigned long)&irq13, 0x08, 0x8E);
	IDTSetGate(46, (unsigned long)&irq14, 0x08, 0x8E);
	IDTSetGate(47, (unsigned long)&irq15, 0x08, 0x8E);
}




unsigned char *exceptionMessages[] = 
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault Exception",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void FaultHandler(struct regs *r)
{
	if(r->intNo < 32)
	{
		print("Unhandled Exception: ");
		print(exceptionMessages[r->intNo]);
        while(1) {
            __asm__("hlt");
        }
	} 

	
	
}

void IRQInstallRoutine(unsigned char irqno, IRQRoutine handler )
{
	handlers[irqno] = handler;
}

void IRQHandler(struct regs *r)
{
	if(r->intNo > 31 && r->intNo < 48 && handlers[r->intNo-32])
	{
		handlers[r->intNo-32](r);
	}
	if(r->intNo >=40)
	{
		out(0xA0, 0x20);
	}
	out(0x20,0x20);
}