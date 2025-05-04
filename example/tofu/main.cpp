// Copyright (c) 2025 Clownacy

// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.

// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <array>
#include <atomic>

#include <clownmdsdk.h>

#include "coordinate.h"
#include "controller.h"
#include "level.h"
#include "objects.h"
#include "sprite.h"

using namespace ClownMDSDK::MainCPU;

static std::atomic<bool> waiting_for_vertical_interrupt;

// M68000 exception handlers.
void _BusErrorHandler()
{

}

void _AddressErrorHandler()
{

}

void _IllegalInstructionHandler()
{

}

void _DivisionByZeroHandler()
{

}

void _CHKHandler()
{

}

void _TRAPVHandler()
{

}

void _PrivilegeViolationHandler()
{

}

void _TraceHandler()
{

}

void _UnimplementedInstructionLineAHandler()
{

}

void _UnimplementedInstructionLineFHandler()
{

}

void _UnassignedHandler()
{

}

void _UninitialisedInterruptHandler()
{

}

void _SpuriousInterruptHandler()
{

}

void _Level1InterruptHandler()
{

}

void _Level2InterruptHandler()
{

}

void _Level3InterruptHandler()
{

}

void _Level4InterruptHandler()
{

}

void _Level5InterruptHandler()
{

}

// Runs once per frame, either 50 or 60 times a second for PAL or NTSC respectively.
void _Level6InterruptHandler()
{
	if (!waiting_for_vertical_interrupt)
		return;

	waiting_for_vertical_interrupt = false;

	Controller::manager.Update();

	Z80::Bus z80_bus;
	Sprite::UploadTable(z80_bus);

	VDP::SendCommand(VDP::RAM::VSRAM, VDP::Access::WRITE, 0);
	VDP::Write(VDP::DataValueWord(Level::camera.y));

	VDP::SendCommand(VDP::RAM::VRAM, VDP::Access::WRITE, 0xDC00);
	VDP::Write(VDP::DataValueWord(-Level::camera.x));

	Level::Redraw(z80_bus);
}

void _Level7InterruptHandler()
{

}

void _TRAP0Handler()
{

}

void _TRAP1Handler()
{

}

void _TRAP2Handler()
{

}

void _TRAP3Handler()
{

}

void _TRAP4Handler()
{

}

void _TRAP5Handler()
{

}

void _TRAP6Handler()
{

}

void _TRAP7Handler()
{

}

void _TRAP8Handler()
{

}

void _TRAP9Handler()
{

}

void _TRAP10Handler()
{

}

void _TRAP11Handler()
{

}

void _TRAP12Handler()
{

}

void _TRAP13Handler()
{

}

void _TRAP14Handler()
{

}

void _TRAP15Handler()
{

}

static void WaitForVerticalInterrupt()
{
	waiting_for_vertical_interrupt = true;
	asm("stop #0x2000");
}

// Run indefinitely; should not return. Handles the bulk of operations.
void _EntryPoint()
{
	VDP::Register01 vdp_register_01{.enable_display = false, .enable_vertical_interrupt = true, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	VDP::Write(vdp_register_01);

	// Upload stuff to the VDP.
	{
		Z80::Bus z80_bus;

		static const auto tiles = std::to_array<unsigned char>({
			#embed "assets/tiles.unc"
		});
		z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL * 4, std::data(tiles), std::size(tiles) / sizeof(short));

		static const auto palette = std::to_array<unsigned char>({
			#embed "assets/palette.unc"
		});
		z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::CRAM, 0, std::data(palette), std::size(palette) / sizeof(short));

		// Draw level.
		Level::Redraw(z80_bus);
	}

	vdp_register_01.enable_display = true;
	VDP::Write(vdp_register_01);

	Objects::AllocateFront<Objects::Player>(Coordinate::Block(7, 3));

	for (;;)
	{
		Objects::Update();
		WaitForVerticalInterrupt();
	}
}
