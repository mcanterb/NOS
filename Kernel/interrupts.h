

struct regs
{
	unsigned int gs, fs;
	unsigned int edi, esi, edp, esp, ebx, edx, ecx, eax;
	unsigned int intNo, error;
	unsigned int eip, cs, eflags, useresp, ss;
};

typedef void (*IRQRoutine)(struct regs *r);

void IRQInstallRoutine(unsigned char irqno, IRQRoutine handler);
void ISRInstall();
void IRQInstall();

volatile extern void isr0();
volatile extern void isr1();
volatile extern void isr2();
volatile extern void isr3();
volatile extern void isr4();
volatile extern void isr5();
volatile extern void isr6();
volatile extern void isr7();
volatile extern void isr8();
volatile extern void isr9();
volatile extern void isr10();
volatile extern void isr11();
volatile extern void isr12();
volatile extern void isr13();
volatile extern void isr14();
volatile extern void isr15();
volatile extern void isr16();
volatile extern void isr17();
volatile extern void isr18();
volatile extern void isr19();
volatile extern void isr20();
volatile extern void isr21();
volatile extern void isr22();
volatile extern void isr23();
volatile extern void isr24();
volatile extern void isr25();
volatile extern void isr26();
volatile extern void isr27();
volatile extern void isr28();
volatile extern void isr29();
volatile extern void isr30();
volatile extern void isr31();
volatile extern void irq0();
volatile extern void irq1();
volatile extern void irq2();
volatile extern void irq3();
volatile extern void irq4();
volatile extern void irq5();
volatile extern void irq6();
volatile extern void irq7();
volatile extern void irq8();
volatile extern void irq9();
volatile extern void irq10();
volatile extern void irq11();
volatile extern void irq12();
volatile extern void irq13();
volatile extern void irq14();        
volatile extern void irq15();