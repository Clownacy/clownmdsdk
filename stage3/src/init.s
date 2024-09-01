| Copyright (c) 2024 Clownacy

| Permission to use, copy, modify, and/or distribute this software for any
| purpose with or without fee is hereby granted.

| THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
| REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
| AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
| INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
| LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
| OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
| PERFORMANCE OF THIS SOFTWARE.

	dc.l	0x00000000,.Lentry,BusErrorHandler,AddressErrorHandler
	dc.l	IllegalInstructionHandler,DivisionByZeroHandler,CHKHandler,TRAPVHandler
	dc.l	PrivilegeViolationHandler,TraceHandler,UnimplementedInstructionLineAHandler,UnimplementedInstructionLineFHandler
	dc.l	UnassignedHandler,UnassignedHandler,UnassignedHandler,UninitialisedInterruptHandler
	dc.l	UnassignedHandler,UnassignedHandler,UnassignedHandler,UnassignedHandler
	dc.l	UnassignedHandler,UnassignedHandler,UnassignedHandler,UnassignedHandler
	dc.l	SpuriousInterruptHandler,Level1InterruptHandler,Level2InterruptHandler,Level3InterruptHandler
	dc.l	Level4InterruptHandler,Level5InterruptHandler,Level6InterruptHandler,Level7InterruptHandler
	dc.l	TRAP0Handler,TRAP1Handler,TRAP2Handler,TRAP3Handler
	dc.l	TRAP4Handler,TRAP5Handler,TRAP6Handler,TRAP7Handler
	dc.l	TRAP8Handler,TRAP9Handler,TRAP10Handler,TRAP11Handler
	dc.l	TRAP12Handler,TRAP13Handler,TRAP14Handler,TRAP15Handler
	dc.l	UnassignedHandler,UnassignedHandler,UnassignedHandler,UnassignedHandler
	dc.l	UnassignedHandler,UnassignedHandler,UnassignedHandler,UnassignedHandler
	dc.l	UnassignedHandler,UnassignedHandler,UnassignedHandler,UnassignedHandler
	dc.l	UnassignedHandler,UnassignedHandler,UnassignedHandler,UnassignedHandler

	.ascii	"SEGA MEGA DRIVE " | Console name
	.ascii	"(C)NAME YEAR.MON" | Copyright
	.ascii	"JAPANESE        " | Japanese name
	.ascii	"  NAME GOES HERE"
	.ascii	"                "
	.ascii	"INTERNATIONAL   " | International name
	.ascii	"  NAME GOES HERE"
	.ascii	"                "
	.ascii	"GM 00000000-00"   | Serial code and revision
	dc.w	0x0000             | Checksum
	.ascii	"J               " | IO support
	dc.l	0x00000000         | Start of ROM
	dc.l	0x003FFFFF         | End of ROM
	dc.l	0x00FF0000         | Start of RAM
	dc.l	0x00FFFFFF         | End of RAM
	.ascii	"    "             | Backup RAM ID
	dc.l	0x20202020         | Start of Backup RAM
	dc.l	0x20202020         | End of Backup RAM
	.ascii	"            "     | Modem
	.ascii	"        "         | Notes
	.ascii	"                "
	.ascii	"                "
	.ascii	"F               " | Region

.Ldata:
	dc.w	0x8000
	dc.w	0x100

	dc.l	0
	dc.l	0xA00000
	dc.l	0xC00000
	dc.l	0xA11200
	dc.l	0xC00004
	dc.l	0xA11100

.Ldata_psg:
	| PSG silence values.
	dc.b	0x9F,0xBF,0xDF,0xFF
.Ldata_psg_end:

.Ldata_vdp:
	| VDP registers values.
	dc.b	0x04 | Register 0x80
	dc.b	0x14 | Register 0x81
	dc.b	0x30 | Register 0x82
	dc.b	0x3C | Register 0x83
	dc.b	0x07 | Register 0x84
	dc.b	0x6C | Register 0x85
	dc.b	0x00 | Register 0x86
	dc.b	0x00 | Register 0x87
	dc.b	0x00 | Register 0x88
	dc.b	0x00 | Register 0x89
	dc.b	0xFF | Register 0x8A
	dc.b	0x00 | Register 0x8B
	dc.b	0x81 | Register 0x8C
	dc.b	0x37 | Register 0x8D
	dc.b	0x00 | Register 0x8E
	dc.b	0x01 | Register 0x8F
	dc.b	0x01 | Register 0x90
	dc.b	0x00 | Register 0x91
	dc.b	0x00 | Register 0x92
	dc.b	0xFF | Register 0x93
	dc.b	0xFF | Register 0x94
	dc.b	0x00 | Register 0x95
	dc.b	0x00 | Register 0x96
	dc.b	0x80 | Register 0x97
.Ldata_vdp_end:

.Ldata_z80:
	.incbin "z80/init.bin"
.Ldata_z80_end:

	align 2

.Lentry:
	| Skip all of this if the console was soft-reset.
	| TODO: Shorten these branches.
	tst.l	(0xA10008).l
	bne.w	.Lsoft_reset
	tst.w	(0xA1000C).l
	bne.w	.Lsoft_reset
	
	lea	.Ldata(%pc),%a5
	movem.w	(%a5)+,%d5/%d6
	movem.l	(%a5)+,%d7/%a1/%a2/%a3/%a4/%a6
	
	| Request Z80 bus.
	move.w	%d6,(%a6)
	| Deassert Z80 reset so that the bus request can complete.
	move.w	%d6,(%a3)
	| We assume that the request has completed by the time the Z80 bus is used later.

	| Satisfy the TMSS lockout system.
	move.w	0xA10000-0xA11100(%a6),%d0
	andi.w	#0xF,%d0
	beq.s	1f
	move.l	#0x53454741,0xA14000-0xA11100(%a6)
1:

	| Read the VDP control port to reset the VDP to a sane default state.
	tst.w	(%a4)

	| Silence PSG.
	moveq	#.Ldata_psg_end-.Ldata_psg-1,%d0
1:	move.b	(%a5)+,0xC00011-0xC00004(%a4)
	dbf	%d0,1b

	| There is no need to silence the YM2612 as it resets itself to sane
	| defaults when the Z80 is reset.

	| Initialise VDP registers.
	moveq	#.Ldata_vdp_end-.Ldata_vdp-1,%d0
1:	move.b	(%a5)+,%d5
	move.w	%d5,(%a4)
	add.w	%d6,%d5
	dbf	%d0,1b

	| Use a DMA Fill to clear VRAM.
	| Registers 8F, 93, 94, and 97 were set earlier.
	move.l	#0x40000080,(%a4)
	move.w	%d7,(%a2)

	| Copy initial Z80 program.
	moveq	#.Ldata_z80_end-.Ldata_z80-1,%d0
1:	move.b	(%a5)+,(%a1)+
	dbf	%d0,1b

	| Toggle the Z80 reset and release its bus.
	move.w	%d7,(%a3)
	move.w	%d7,(%a6)
	move.w	%d6,(%a3)

	| Clear USP.
	movea.l	%d7,%a0
	move.l	%a0,%usp

	| Clear RAM.
	move.w	#64*1024/16-1,%d0
1:	move.l	%d7,-(%a0)
	move.l	%d7,-(%a0)
	move.l	%d7,-(%a0)
	move.l	%d7,-(%a0)
	dbf	%d0,1b

	| Hopefully, the DMA Fill has completed by now.
	| Disable DMA operations, and set the auto-increment to
	| 2 bytes for the following clears.
	move.l	#0x81048F02,(%a4)

	| Clear CRAM.
	move.l	#0xC0000000,(%a4)
	moveq	#16*4/2-1,%d0
1:	move.l	%d7,(%a2)
	dbf	%d0,1b

	| Clear VSRAM.
	move.l	#0x40000010,(%a4)
	moveq	#40/2-1,%d0
1:	move.l	%d7,(%a2)
	dbf	%d0,1b

	| Load DATA section.
	lea	(_DATA_ROM_START_).l,%a0
	lea	(_DATA_RAM_START_).l,%a1
	move.w	#_DATA_LOOP_COUNT_,%d0
	moveq	#_DATA_LOOP_OFFSET_,%d1
	jmp	2f(%pc,%d1.w)

1:	move.l	(%a0)+,(%a1)+
	move.l	(%a0)+,(%a1)+
	move.l	(%a0)+,(%a1)+
	move.l	(%a0)+,(%a1)+
2:
	dbf	%d0,1b

	| Run global constructors.
	jsr	_init

.Lsoft_reset:
	| Wait for any in-progress DMA operations to end, to avoid race-conditions.
	| This has the nice side-effect of reading the VDP control port to reset any
	| operations that it may have been in the middle of when the console was reset.
1:	btst	#1,(0xC00005).l
	bne.s	1b

	| Jump into the user-code.
	jmp	(EntryPoint).l
