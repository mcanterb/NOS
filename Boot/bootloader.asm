[BITS 16]       ; We need 16-bit intructions for Real mode



[ORG 0x7C3E]    
		mov ax,0x0000
		mov ds,ax
		mov ss,ax
		mov sp, 0xC000
		
	
		mov dl, 0x0000
		mov ah, 0x01
		int 0x13
		or al,al
		jz driveready
		jmp $
driveready:
		mov ax, 0x020E  ; Load the root directory into memory.
		mov cx, 0x0002  
		mov dx, 0x0100
		mov bx, 0x0000
		mov es, bx
		mov bx, 0x2000
		int 0x13
		
		jnc drivesuccess
		mov si, ReadFail
		call PutStr
		jmp $
drivesuccess:
		mov bx, 0x2000
		cld
FindLoader:
		mov cx, 0x000B
		mov si, bx
		mov di,SecondStage
		cmp bx, 0x3C00
		je notfound
		repe 
		cmpsb

		jz found
		add bx, 0x0020
		jmp FindLoader	
notfound:
		mov si,Fail
		call PutStr
		jmp $
found:
		mov [BOOTBIN], bx
		;Now Open Gate A20
		cli
waitForKbd1:
		xor ax,ax
		in al, 0x64
		bt ax, 1
		jc waitForKbd1
		
		mov al, 0x0d0
		out 0x64, al

waitForKbd2:
		xor ax,ax
		in al,0x64
		bt ax, 0
		jnc waitForKbd2
		xor ax,ax
		in al, 0x60
		push ax
waitForKbd3:
		in al,0x64
		bt ax,1
		jc waitForKbd3
		
		mov al, 0xD1
		out 0x64, al
		
waitForKbd4:
		xor ax,ax
		in al, 0x64
		bt ax,1
		jc waitForKbd4
		
		pop ax
		or al, 0x02
		out 0x60,al
		
waitForKbd5:
		xor ax,ax
		in al, 0x64
		bt ax,1
		jc waitForKbd5
		
		mov al, 0xD0
		out 0x64, al
		
waitForKbd6:
		xor ax,ax
		in al, 0x64
		bt ax, 0
		jnc waitForKbd6
		
		xor ax,ax
		in al, 0x60
		
		bt ax, 1
		
		sti
		jc A20Open
		
		mov si,A20Fail
		call PutStr
		jmp $

A20Open:		

		mov bx, [BOOTBIN]
		add bx, 0x001a
		mov ax, [bx]
		add ax, 0x1F
		call LBAToCHS
		mov ax, 0x0201
		mov bx, 0x8000
		mov ch, [CYL]
		mov cl, [SECT]
		mov dh, [HEAD]
		mov dl, 0x00
		int 0x13
		
		jnc drivesuccess2
		mov si, ReadFail
		call PutStr
		jmp $

drivesuccess2:		

		jmp 0x8000
	
		
		jmp $



		
PutStr:
		mov ah, 0x0E
		mov bh, 0x00
		mov bl, 0x07
		
.nextchar:
		lodsb
		or al,al
		jz .return
		
		int 0x10
		jmp .nextchar
.return:
		ret

LBAToCHS:	
		xor DX, DX
        mov BX, 0x12
        div BX
        inc DX
        mov [SECT], DL
        xor DX, DX
        mov BX, 0x0002
        div BX
        mov [CYL], AL
        mov [HEAD], DL
		ret

		
		
		
		
		
		
			
Fail db 'Cannot find BOOT.BIN',13,10,0
A20Fail db 'Could not Open A20 Gate',13,10,0
ReadFail db 'Failed to Read Media',13,10,0 
SecondStage db 'BOOT    BIN'
		
times 510-($-$$) db 0	; Fill the rest of the files with zeros, until we reach 510 bytes

        dw 0AA55h		; Our boot sector identifyer

SECT equ 0x1000
HEAD equ 0x1001
CYL  equ 0x1002
BOOTBIN equ 0x1004
