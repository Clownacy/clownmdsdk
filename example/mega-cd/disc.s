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

	.section  .text.init.header

	| Disc header
	.ascii	"SEGADISCSYSTEM  "
	| Volume
	.asciz	"SOUND TEST "
	dc.w	1	| CD-ROM
	dc.w	0x100	| Version 1.0
	| System
	.asciz	"SOUND TEST "
	dc.w	0
	dc.w	0
	| Initial program
	dc.l	0x800		| Start
	dc.l	0x8000		| Length
	dc.l	0		| Offset
	dc.l	0		| Work RAM
	| Sub program
	dc.l	0x8800		| Start
	dc.l	0x7800		| Length
	dc.l	0		| Offset
	dc.l	0		| Work RAM
	| Filler
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	| Regular header
	.ascii	"SEGA MEGA DRIVE "	| System
	.ascii	"(C)CLWN 2023.SEP"	| Copyright
	.ascii	"SOUND TEST      "	| Domestic (Japan) name
	.ascii	"                "
	.ascii	"                "
	.ascii	"SOUND TEST      "	| International name
	.ascii	"                "
	.ascii	"                "
	.ascii	"GM T-XXXXX -00  "	| Serial code and revision
	.ascii	"J               "	| IO support
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"                "
	.ascii	"U               "	| Region

	.org	0x200
	.incbin	"bin/ip.bin"

	.org	0x8800
	.incbin	"bin/sp.bin"

	.fill	0x8000,1,0
