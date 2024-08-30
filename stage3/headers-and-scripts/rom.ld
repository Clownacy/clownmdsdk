STARTUP(../../../../init.o)
INPUT(
	../../../../crti.o
	../../../../lib/gcc/m68k-elf/14.2.0/crtbegin.o
	../../../../lib/gcc/m68k-elf/14.2.0/crtend.o
	../../../../crtn.o
)
OUTPUT_FORMAT(binary)

MEMORY
{
	ROM (rx) : ORIGIN = 0x00000000, LENGTH = 4M
	RAM (wx) : ORIGIN = 0xFFFF0000, LENGTH = 64K
}

SECTIONS
{
	.rom() : {
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
		. = ALIGN(4);
		*(.ctors)
		. = ALIGN(4);
		*(.init)
	} > ROM

	.ram : {
		. = ALIGN(2);
_DATA_ROM_START_ = LOADADDR(.ram);
_DATA_RAM_START_ = .;
		*(.data)
		*(.data.*)
		. = ALIGN(4);
_DATA_RAM_END_ = .;
_DATA_SIZE_ = ABSOLUTE(_DATA_RAM_END_ - _DATA_RAM_START_);
		. = ALIGN(2);
_BSS_START_ = .;
		*(.bss)
		*(.bss.*)
		. = ALIGN(4);
_BSS_END_ = .;
_BSS_SIZE_ = ABSOLUTE(_BSS_END_ - _BSS_START_);
	} > RAM AT> ROM

	/DISCARD/ : {
		*(.dtors)
		*(.fini)
		*(.eh_frame)
		*(.tm_clone_table)
		*(.comment)
		*(.debug_str)
		*(.debug_line)
		*(.debug_line_str)
		*(.debug_info)
		*(.debug_abbrev)
		*(.debug_aranges)
	}
}
