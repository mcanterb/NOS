#include "interrupts.h"
#include "system.h"
#include "video.h"
#include "utility.h"
#include "timer.h"
#include "kmalloc.h"

static volatile unsigned int __ticks;
static volatile Timer *__timers;

void TimerHandler(struct regs *r)
{
	if(__ticks > 0) __ticks--;
    int i;
    char buffer[12];
    for(i = 0; i < 256; i++)
    {   
        if(__timers[i].schedCallback != null && __timers[i].ticks > 0)
        {

                
            if(__timers[i].ticks == 0)
            {
                itoa(i, buffer);
                print("Timer Firing: ");
                print(buffer);
                print(" : ");
                itoh((unsigned int)__timers[i].schedCallback, buffer);
                print(buffer);
                print("\n");
                __timers[i].schedCallback(__timers[i].param);
            }
        }
    }
}

int NewTimer(unsigned int ticks, TimerCallback schedCallback, int param)
{
    int i = 0;
    while(__timers[i].schedCallback != null )
    {
        i++;
    }
    __timers[i].ticks = ticks;
    __timers[i].schedCallback = schedCallback;
    __timers[i].param = param;
    return i;
}

void DestroyTimer(int timer)
{
    if(timer < 0 || timer > 255) return;
    __timers[timer].schedCallback = null;
}

void ResetTimer(int timer, unsigned int ticks)
{
    if(timer < 0 || timer > 255) return;
    __timers[timer].ticks = ticks;
}

void InitTimer(int frequency)
{
	int divisor = 1193180 / frequency;
	out(0x43, 0x36);
	out(0x40, (unsigned char)(divisor& 0xFF));
	out(0x40, (unsigned char)(divisor >> 8));
    __ticks = 0;
    __timers = kmalloc(sizeof(Timer)*256);
    memset(__timers, 0, sizeof(Timer)*256);
	char buffer[12];
    itoh(__timers, buffer);
    print("Timers allocated at: ");
    print(buffer);
    print("\n");
    IRQInstallRoutine(0,&TimerHandler);
}

void sleep(unsigned int ticks)
{
	__ticks = ticks+1;
	while(__ticks > 0)
    {
        __asm__("hlt");
    }
}