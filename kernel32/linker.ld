ENTRY(_start)

SECTIONS {
	. = 0x100000;
	_kernel_start = .;
	.text :
	{
		*(.multiboot2)
		*(.text)
		*(.rodata)
	}
	.data : { *(.data) }
	.bss : { *(.bss) }
	_kernel_end = .;
}
