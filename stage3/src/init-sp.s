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

	.section  .text.init

.header:
	.asciz	"MAIN       "
	dc.w	0x100,0
	dc.l	0
	dc.l	0
	dc.l	.offsets-.header
	dc.l	0
.offsets:
	dc.w	.init-.offsets
	dc.w	_SP_Main-.offsets
	dc.w	_SP_VerticalInterrupt-.offsets
	dc.w	_SP_User-.offsets
	dc.w	0

.init:
	.include "clear-bss.s"

	| Fall-through to the compiled code...
