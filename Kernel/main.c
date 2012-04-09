#include "video.h"
#include "system.h"
#include "interrupts.h"
#include "keyboard.h"
#include "memman.h"
#include "kmalloc.h"
#include "drivers/floppy.h"

const char *hello;
const unsigned int *kernelSize = (unsigned int *)0xF0001804;

void main()
{
	clrscr();
	print(hello);
	IDTInstall();
	IRQInstall();
    MMInit();
	InitTimer(100);
    STI();
	InitKeyboard();
	InitFloppy();
    unsigned char buffer[20];
	print("Kernel Size: ");
	itoa((*kernelSize)>>1, buffer);
	print(buffer);
	print("KB\n");
	
	itoa((MMGetTotal()/1024), buffer);
	print(buffer);
	print("KB Ram Total\n");
	itoa((MMGetAvailable()/1024), buffer);
	print(buffer);
	print("KB Ram Available to NOS\n");
	print("Floppy Drives Detected:\n");
	print("Primary: ");
	print(GetFloppyDriveType(0));
    print("\n");
	print("Secondary: ");
	print(GetFloppyDriveType(1));
    print("\n");
	
    while(1)
    {
        __asm__("hlt");
    }
}


const char *hello = "NOS v0.0.1\n";