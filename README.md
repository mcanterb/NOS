## NoS

NoS is a hobby OS I'm writing in my spare time. It currently features a custom bootloader, floppy disk drivers, keyboard drivers,
Memory manager, kmalloc / free equivalent for drivers, and more.

# Requirements

NoS uses make, gcc, fasm, nasm, and probably more unix utilities. 
Virtual Floppy Drive ([VFD](http://sourceforge.net/projects/vfd/)) or something similar
Some parts of the makefile assume that vfd has a virtual 1.44 meg floppy mounted at B:\. 

# Compiling

The kernel can be compiled using make. Note that currently all drivers are linked into the kernel. 

Run the following to compile and link the kernel:

	mkdir bin
	mkdir bin/drivers
	make bin/MYOS.BIN

This will create the kernel in bin/MYOS.BIN. This is because it used to be called MYOS. This should probably be fixed at some point.

# Building the Bootloader

The bootloader can be built using the boot option of the makefile. This assumes a certain set up however. 
You can build the second stage bootloader with the following command however:
	
	nasm -f bin -o bin/BOOT.BIN boot/bootloader_stage2.asm

In any case, the first stage bootloader must be built manually using this command:

	nasm -f bin -o bin/bootloader.bin boot/bootloader.asm

It should result in a file that is exactly 512 bytes and will act as a MBR.

#Building the Floppy Image

This has been tested using VFD. However, any other method of generating a raw image that is 1.44Megs would work.
The first stage bootloader assumes a FAT16 file system. It searches for a file named BOOT.BIN and loads it.
The second stage bootloader in BOOT.BIN then searches for MYOS.BIN and loads the entire kernel into memory. It sets up 32 bit protected mode and then jumps to the kernel.

Therefore, the requirements are to build a FAT16 image that is 1.44 megs, contains BOOT.BIN and MYOS.BIN, and has the MBR replaced with bootloader.bin.

There is a utility program called injectboot that was written for this purpose. This program can be compiled like any other c program. Use the following command:

	gcc Utility/injectboot.c -o bin/injectboot.exe 

This is Windows specific of course. You should use what is appropriate for your OS. Then you can use InjectBoot to install the MBR in the floppy image like so:

	injectboot bin/bootloader.bin NoS.img

Of course, you would replace NoS.img with the path to your floppy image. Then this file can be mounted in VirtualBox or VMWare and booted.



