STARTUP(../../../../init-ip.o)
 # This is needed so that _EntryPoint is not
 # optimised-away due to being "unused".
ENTRY(_EntryPoint)
OUTPUT_FORMAT(binary)

MEMORY
{
	ROM (rx) : ORIGIN = 0x200, LENGTH = 32K
	RAM (rwx) : ORIGIN = 0xFFFF0000, LENGTH = 0xFC00
}

SECTIONS
{
	.ram : {
		*(.text.init)  # init-ip.s
		*(.init)       # Global constructors
		*(.text.entry) # _EntryPoint
		*(.text)
		*(.text.*)
		*(.rodata)
		*(.rodata.*)
		. = ALIGN(4);
		*(.ctors)
		. = ALIGN(4);
		. = ALIGN(2);
_DATA_START_ = .;
		*(.data)
		*(.data.*)
		. = ALIGN(4);
_DATA_END_ = .;
_DATA_SIZE_ = ABSOLUTE(_DATA_END_ - _DATA_START_);
_DATA_LOOP_COUNT_ = ABSOLUTE(_DATA_SIZE_ / 16);
_DATA_LOOP_OFFSET_ = ABSOLUTE(-(_DATA_SIZE_ % 16 / 4 * 2));
		. = ALIGN(2);
	} > RAM AT> ROM

	.bss : {
_BSS_START_ = .;
		*(.bss)
		*(.bss.*)
		. = ALIGN(4);
_BSS_END_ = .;
_BSS_SIZE_ = ABSOLUTE(_BSS_END_ - _BSS_START_);
_BSS_LOOP_COUNT_ = ABSOLUTE(_BSS_SIZE_ / 16);
_BSS_LOOP_OFFSET_ = ABSOLUTE(-(_BSS_SIZE_ % 16 / 4 * 2));
	} > RAM

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
