/*
 * Copyright (c) 2026 ilizavr
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include "interrupt.h"
#include "../lib/ports.h"
#include "panic.h"

void pic_remap()
{
	//init master and slave
	outb(0x20,0x11);
	outb(0xA0,0x11);
	
	//mov irq to 0x20-0x2F
	outb(0x21,0x20);
	outb(0xA1,0x28);

	//master-slave connection
	outb(0x21,0x04);
	outb(0xA1,0x02);

	//8086 mode
	outb(0x21,0x01);
	outb(0xA1,0x01);

	//enable interrupts
	outb(0x21,0);
	outb(0xA1,0);
}

PAK struct idt_entry
{
	u16 low_offset;
	u16 sel;
	u8 always0;
	u8 flags;
	u16 high_offset;
};
PAK struct idt_ptr
{
	u16 limit;
	u16 base_low;
	u16 base_high;
};

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void _none_interrupt();
extern void _pic_interrupt_master();
extern void _pic_interrupt_slave();


void set_idt_gate(u8 num,void* fnc)
{
	u32 base = (u32)fnc;
	idt[num].low_offset = base&0xFFFF;
	idt[num].high_offset = (base>>16)&0xFFFF;
	idt[num].sel = 0x10;
	idt[num].always0 = 0;
	idt[num].flags = 0x8E;//ring0 32bit
}


cpu_exception(0)
cpu_exception(1)
cpu_exception(2)
cpu_exception(3)
cpu_exception(4)
cpu_exception(5)
cpu_exception(6)
cpu_exception(7)
cpu_exception(8)
cpu_exception(9)
cpu_exception(10)
cpu_exception(11)
cpu_exception(12)
cpu_exception(13)
cpu_exception(14)
cpu_exception(15)
cpu_exception(16)
cpu_exception(17)
cpu_exception(18)
cpu_exception(19)
cpu_exception(20)
cpu_exception(21)
cpu_exception(22)
cpu_exception(23)
cpu_exception(24)
cpu_exception(25)
cpu_exception(26)
cpu_exception(27)
cpu_exception(28)
cpu_exception(29)
cpu_exception(30)
cpu_exception(31)
cpu_exception(32)


void init_idt()// ONLY32BIT
{
	idtp.limit = 256*sizeof(struct idt_entry)-1;
	idtp.base_low = (u16)&idt;
	idtp.base_high = (u32)&idt>>16;
	
	cpu_exception_init(0);
	cpu_exception_init(1);
	cpu_exception_init(2);
	cpu_exception_init(3);
	cpu_exception_init(4);
	cpu_exception_init(5);
	cpu_exception_init(6);
	cpu_exception_init(7);
	cpu_exception_init(8);
	cpu_exception_init(9);
	cpu_exception_init(10);
	cpu_exception_init(11);
	cpu_exception_init(12);
	cpu_exception_init(13);
	cpu_exception_init(14);
	cpu_exception_init(15);
	cpu_exception_init(16);
	cpu_exception_init(17);
	cpu_exception_init(18);
	cpu_exception_init(19);
	cpu_exception_init(20);
	cpu_exception_init(21);
	cpu_exception_init(22);
	cpu_exception_init(23);
	cpu_exception_init(24);
	cpu_exception_init(25);
	cpu_exception_init(26);
	cpu_exception_init(27);
	cpu_exception_init(28);
	cpu_exception_init(29);
	cpu_exception_init(30);
	cpu_exception_init(31);
	cpu_exception_init(32);

	for(int i = 0x20;i<0x28;i++) set_idt_gate(i,_pic_interrupt_master);
	for(int i = 0x28;i<0x30;i++) set_idt_gate(i,_pic_interrupt_slave);
	for(int i = 0x30;i<256;i++) set_idt_gate(i,_none_interrupt);

	asm volatile("lidt (%0)" : : "r" (&idtp));
}

void pic_eoi()
{
	outb(0x20,0x20);
}
