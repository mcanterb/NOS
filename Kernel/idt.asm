;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;This assembly file provides the neccessary functions needed
;for MyOS to install its Interrupt Service Routines.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
format MS COFF
public _isr0
public _isr1
public _isr2
public _isr3
public _isr4
public _isr5
public _isr6
public _isr7
public _isr8
public _isr9
public _isr10
public _isr11
public _isr12
public _isr13
public _isr14
public _isr15
public _isr16
public _isr17
public _isr18
public _isr19
public _isr20
public _isr21
public _isr22
public _isr23
public _isr24
public _isr25
public _isr26
public _isr27
public _isr28
public _isr29
public _isr30
public _isr31
public _IDTLoad
extrn _FaultHandler
extrn _idtp

_isr0:
	cli
	push 0
	push 0
	jmp isr_common

_isr1:
	cli
	push 0
	push 1
	jmp isr_common
	
_isr2:
	cli
	push 0
	push 2
	jmp isr_common

_isr3:
	cli
	push 0
	push 3
	jmp isr_common
	
_isr4:
	cli
	push 0
	push 4
	jmp isr_common

_isr5:
	cli
	push 0
	push 5
	jmp isr_common
	
_isr6:
	cli
	push 0
	push 6
	jmp isr_common

_isr7:
	cli
	push 0
	push 7
	jmp isr_common
	
_isr8:
	cli
	push 8
	jmp isr_common

_isr9:
	cli
	push 0
	push 9
	jmp isr_common

_isr10:
	cli
	push 10
	jmp isr_common

_isr11:
	cli
	push 11
	jmp isr_common
	
_isr12:
	cli
	push 12
	jmp isr_common

_isr13:
	cli
	push 13
	jmp isr_common
	
_isr14:
	cli
	push 14
	jmp isr_common

_isr15:
	cli
	push 0
	push 15
	jmp isr_common
	
_isr16:
	cli
	push 0
	push 16
	jmp isr_common

_isr17:
	cli
	push 0
	push 17
	jmp isr_common
	
_isr18:
	cli
	push 0
	push 18
	jmp isr_common

_isr19:
	cli
	push 0
	push 19
	jmp isr_common

_isr20:
	cli
	push 0
	push 20
	jmp isr_common

_isr21:
	cli
	push 0
	push 21
	jmp isr_common
	
_isr22:
	cli
	push 0
	push 22
	jmp isr_common

_isr23:
	cli
	push 0
	push 23
	jmp isr_common
	
_isr24:
	cli
	push 0
	push 24
	jmp isr_common

_isr25:
	cli
	push 0
	push 25
	jmp isr_common
	
_isr26:
	cli
	push 0
	push 26
	jmp isr_common

_isr27:
	cli
	push 0
	push 27
	jmp isr_common
	
_isr28:
	cli
	push 0
	push 28
	jmp isr_common

_isr29:
	cli
	push 0
	push 29
	jmp isr_common

_isr30:
	cli
	push 0
	push 30
	jmp isr_common

_isr31:
	cli
	push 0
	push 31
	jmp isr_common
	
	
	
	
	
isr_common:
	pusha
;    push ds
;    push es
    push fs
    push gs
    mov ax, 0x10   
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   
    push eax
    mov eax, _FaultHandler
    call eax       
    pop eax
    pop gs
    pop fs
;    pop es
;    pop ds
    popa
    add esp, 8     
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
	
_IDTLoad:
	lidt [_idtp]
	ret

public _irq0
public _irq1
public _irq2
public _irq3
public _irq4
public _irq5
public _irq6
public _irq7
public _irq8
public _irq9
public _irq10
public _irq11
public _irq12
public _irq13
public _irq14           
public _irq15




_irq0:
    cli
    push 0    
    push  32
    jmp irq_common_stub
_irq1:
    cli
    push 0    
    push  33
    jmp irq_common_stub
_irq2:
    cli
    push  0    
    push  34
    jmp irq_common_stub
_irq3:
    cli
    push  0    
    push  35
    jmp irq_common_stub
_irq4:
    cli
    push  0    
    push  36
    jmp irq_common_stub
_irq5:
    cli
    push  0    
    push  37
    jmp irq_common_stub
_irq6:
    cli
    push  0    
    push  38
    jmp irq_common_stub
_irq7:
    cli
    push  0    
    push  39
    jmp irq_common_stub
_irq8:
    cli
    push  0    
    push  40
    jmp irq_common_stub
_irq9:
    cli
    push  0    
    push  41
    jmp irq_common_stub
_irq10:
    cli
    push  0    
    push  42
    jmp irq_common_stub
_irq11:
    cli
    push  0    
    push  43
    jmp irq_common_stub
_irq12:
    cli
    push  0    
    push  44
    jmp irq_common_stub
_irq13:
    cli
    push  0    
    push  45
    jmp irq_common_stub
_irq14:
    cli
    push  0    
    push  46
    jmp irq_common_stub

_irq15:
    cli
    push  0
    push  47
    jmp irq_common_stub

extrn _IRQHandler


irq_common_stub:
    pusha
    ;push ds
    ;push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _IRQHandler
    call eax
	pop eax
	pop gs
    pop fs
    ;pop es
	;add esp, 0x2
	;pop ds
    popa
    add esp, 8
    iret
	
	
