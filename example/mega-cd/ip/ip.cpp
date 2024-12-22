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

namespace MD = ClownMDSDK::MainCPU;

void _EntryPoint()
{
	MD::VDP::Write(MD::VDP::Register01{.enable_display = false, .enable_vertical_interrupt = false, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true});

	// Write colours to CRAM.
	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, 2);
	MD::VDP::Write(MD::VDP::DataValueLongword(MD::VDP::CRAM::Colour{7, 0, 0}, MD::VDP::CRAM::Colour{0, 7, 0}));
	MD::VDP::Write(MD::VDP::CRAM::Colour{0, 0, 7});

	MD::MegaCD::subcpu.bus_request = true;

	for (;;);
}
