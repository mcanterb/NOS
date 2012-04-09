all: install

bin/MYOS.BIN: bin/main.o bin/video.o bin/ports.o  bin/system.o bin/interrupts.o bin/idt.o bin/timer.o bin/keyboard.o bin/utility.o bin/memman.o bin/kmalloc.o bin/drivers/floppy.o
		ld -e _main -Ttext 0x100000 -o bin/kernel.o bin/main.o bin/video.o bin/ports.o  bin/system.o bin/interrupts.o bin/idt.o bin/timer.o bin/keyboard.o bin/utility.o bin/memman.o bin/kmalloc.o bin/drivers/floppy.o
		ld -i -e _main -Ttext 0x100000 -o bin/kernel.o bin/main.o bin/video.o bin/ports.o  bin/system.o bin/interrupts.o bin/idt.o bin/timer.o bin/keyboard.o bin/utility.o bin/memman.o bin/kmalloc.o bin/drivers/floppy.o
		objcopy -R .note -R .comment -S -O binary bin/kernel.o bin/MYOS.BIN 
		
		
bin/main.o: kernel/main.c
		gcc -ffreestanding -c kernel/main.c -o bin/main.o
		
bin/video.o: kernel/video.c
		gcc -c kernel/video.c -o bin/video.o
		
bin/ports.o: kernel/ports.c
		gcc  -c kernel/ports.c -o bin/ports.o
		
bin/system.o: kernel/system.c
		gcc -c kernel/system.c -o bin/system.o

bin/interrupts.o: kernel/interrupts.c
		gcc -c kernel/interrupts.c -o bin/interrupts.o
		
bin/timer.o: kernel/timer.c
		gcc -c kernel/timer.c -o bin/timer.o
		
bin/keyboard.o: kernel/keyboard.c
		gcc -c kernel/keyboard.c -o bin/keyboard.o
		
bin/utility.o: kernel/utility.c
		gcc -c kernel/utility.c -o bin/utility.o		
		
bin/memman.o: kernel/memman.c
		gcc -c kernel/memman.c -o bin/memman.o	

bin/kmalloc.o: kernel/kmalloc.c
		gcc -c kernel/kmalloc.c -o bin/kmalloc.o	
		
bin/drivers/floppy.o: kernel/drivers/floppy.c
		gcc -I kernel -c kernel/drivers/floppy.c -o bin/drivers/floppy.o
		
bin/idt.o:
		fasm  kernel/idt.asm bin/idt.o 
		
		

install: bin/MYOS.BIN
		copy bin\MYOS.BIN B:\ 
		bin\vfd.exe save
		
clean:
		cd bin
		del *.bin
		del *.o
		cd ..
		
boot:   boot/bootloader_stage2.asm
		nasm -f bin -o bin/BOOT.BIN boot/bootloader_stage2.asm
		copy bin\BOOT.BIN B:\ 
		bin\vfd.exe save