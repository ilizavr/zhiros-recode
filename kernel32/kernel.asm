bits 32

MULTIBOOT2_HEADER_MAGIC equ 0xe85250d6

section .multiboot2
 align 8

header_start:
 dd MULTIBOOT2_HEADER_MAGIC
 dd 0
 dd header_end - header_start
 dd 0x100000000 - (MULTIBOOT2_HEADER_MAGIC + 0 + header_end - header_start)

align 8
 dw 5   ; framebuffer request
 dw 0   ; flags
 dd 20  ; size
 dd 800 ; width
 dd 600 ; height
 dd 32  ; color scheme

align 8
 dw 0   ; end
 dw 0
 dd 8
header_end:


section .text
global _start
global _none_interrupt
global _pic_interrupt_master
global _pic_interrupt_slave
global keyboard_isr_handler


extern main
extern pic_eoi
extern keyboard_isr_handler_c

_start:
 cli

 mov esp, stack

 push eax
 push ebx
 call main

.j: hlt
 jmp .j

_pic_interrupt_master:
    cli
    push eax
    mov al, 0x20
    out 0x20, al
    pop eax
    iret


_pic_interrupt_slave:
    cli
    push eax
    mov al, 0x20
    out 0xA0, al
    out 0x20, al
    pop eax
    iret
_none_interrupt:
    iret

keyboard_isr_handler:
    cli
    pusha
    call keyboard_isr_handler_c
    mov eax, 0x20
    out 0x20, al
    popa
    sti
    iret


section .bss
resb 65536
stack:
