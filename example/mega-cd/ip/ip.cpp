// Copyright (c) 2024 Clownacy

// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.

// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <clownmdsdk.h>

using namespace ClownMDSDK;

void _EntryPoint()
{
	VDP::Write(VDP::Register01{.enable_display = false, .enable_vertical_interrupt = false, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true});

	// Write colours to CRAM.
	VDP::SendCommand(VDP::RAM::CRAM, VDP::Access::WRITE, 2);
	VDP::Write(VDP::DataValueLongword(VDP::CRAM::Colour{7, 0, 0}, VDP::CRAM::Colour{0, 7, 0}));
	VDP::Write(VDP::CRAM::Colour{0, 0, 7});

	for (;;);
}
