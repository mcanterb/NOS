[BITS 16]

[ORG 0x8000]
	mov ah, 0x02
	mov al, [0x7C16]
	mov cx, 0x0002
	mov dx, 0x0000
	mov bx, FAT
	int 0x13
	jnc FatLoaded
	mov si, FatFail
	call PutStr
	jmp $
	
FatLoaded:
	mov bx, 0x2000
	
	cld
FindKernel:
	mov cx, 0x000B
	mov si, bx
	mov di, Kernel
	cmp bx, 0x3C00
	je notfound
	repe 
	cmpsb

	jz found
	add bx, 0x0020
	jmp FindKernel	
notfound:
	mov si,KernelNF
	call PutStr
	jmp $
found:
	mov ax, 0x1000
	mov es, ax
	mov ax, [bx+0x1A]	;First Cluster
	mov bx, 0x0000

loadLoop:
	push ax
	add ax, CLUSTEROFF
	call LoadSector
	add bx, 0x0200
	mov si, period
	call PutStr
	inc dword [NUMSECTS]
	inc dword [NUMSECTS2]
	pop ax	
	shr ax, 1
	jc .second
	mov cx, 0x0003 ;first part of packed bits
	mul cx  
	add ax, FAT
	mov bp, ax
	call Unpack
	cmp ax, 0x0FFF
	je  Loaded
	jmp loadLoop
.second: 		;Second part of packed bits
	mov cx, 0x0003
	mul cx 
	add ax, FAT
	mov bp, ax
	call Unpack
	mov ax,cx
	cmp ax, 0x0FFF
	je  Loaded
	jmp loadLoop
	
Loaded:	
	;Get Memory Map
	mov ax, 0
	mov es, ax
	mov edi, MEMORYMAP+4
	mov ax, 0xE820
	mov eax, 0x0000E820
	mov edx, 0x534D4150
	mov ebx, 0x00000000
	mov ecx, 20
	mov ebp, 0
mmaploop:
	int 0x15
	jc SetupGDT
MMContinue:
	cmp ebx, 0
	je SetupGDT
	mov ax, 0xE820
	mov eax, 0x0000E820
	add di, 20
	mov ecx, 20
	inc ebp
	jmp mmaploop

SetupGDT:	
	;Set up GDT

	mov [MEMORYMAP], ebp
	cli
	xor ax,ax
	mov ds, ax
	lgdt [gdt_desc]
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp 0008h:ClearPipe
[BITS 32]
ClearPipe:
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov esp, 0x90000
	mov esi, 0x10000
	mov edi, 0x100000
MoveLoop:
	mov ecx, 0x80
	rep
	movsd
	dec dword [NUMSECTS]
	jmp KernelEntry
	jmp MoveLoop
KernelEntry:
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov esp, 0x90000
	jmp 18h:0xC0000000
	
[BITS 16]
LoadSector:
    pusha
	call LBAToCHS
	mov ax, 0x0201
	mov ch, [CYL]
	mov cl, [SECT]
	mov dh, [HEAD]
	mov dl,  0x00
	int 0x13
	popa
	ret

LBAToCHS:	
		pusha
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
		popa
		ret

Unpack:
		mov ax, [bp]
		and ax, 0x0FFF
		mov cx, [bp+1]
		shr cx, 4
		ret
		
;ax = number
;IntToStr:
;		push ax
;		push bx
;		push dx
;		cmp ax, 10
;		jbe IntToStr2
;		
;		push ax
;		xor dx,dx
;		mov bx, 10
;		div bx
;		call IntToStr
;		pop ax
;		jmp IntToStr3
;IntToStr2:
;		mov bx, BUFFER
;		add bx, cx
;		mov [bx+1], byte 0x00
;IntToStr3:		
;		mov bx, 10
;		xor dx,dx
;		div bx
;		add dx, 0x30
;		mov bx, BUFFER
;		add bx, cx
;		mov [bx], dl
;		inc cx
;		pop dx
;		pop bx
;		pop ax
;		ret



PutStr:
		push ax
		push bx
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
		pop bx
		pop ax
		ret
	
KernelNF db 'Kernel not found.',13,10,0
Kernel db 'MYOS    BIN'
period db '.',0
FatFail db 'Cannot load FAT',13,10,0
MMapFail db 'Memory Map Failure',13,10,0

gdt:                   

gdt_null:              
        dd 0
        dd 0
gdt_code:              
        dw 0FFFFh
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0
gdt_data:               
        dw 0FFFFh
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0
gdt_code_offset:              
        dw 0FFFFh
        dw 0
        db 04010h
        db 10011010b
        db 11001111b
        db 0
gdt_data_offset:               
        dw 0FFFFh
        dw 0
        db 04010h
        db 10010010b
        db 11001111b
        db 0

gdt_end:               
gdt_desc:               
        dw gdt_end - gdt - 1    
        dd gdt                 

	
SECT equ 0x1000
HEAD equ 0x1001
CYL  equ 0x1002
FAT  equ 0x4000
ROOT equ 0x2000
CLUSTEROFF equ 0x1F
INDEX equ 0x1004
BUFFER equ 0x1006
MEMORYMAP equ 0x1000
NUMSECTS equ 0x1800 
NUMSECTS2 equ 0x1804




