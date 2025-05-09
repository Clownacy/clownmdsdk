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
#include <initializer_list>

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

void _ControllerInterruptHandler()
{

}

static unsigned int horizontal_interrupt_flip_flop;

void _HorizontalInterruptHandler()
{
	switch (horizontal_interrupt_flip_flop)
	{
		case 0:
			break;

		case 1:
			// Configure horizontal interrupt.
			VDP::SetHorizontalInterruptInterval(9);
			break;

		case 2:
			VDP::SetWindowPlaneHorizontalConfiguration(true, (Coordinate::screen_size.tiles.x - Coordinate::hud_size.tiles.x) / 2);
			break;

		default:
			// Cut-off Window Plane so that it doesn't go completely down the screen.
			VDP::SetWindowPlaneHorizontalConfiguration(false, 0);
			VDP::Write(VDP::Register00{.blank_leftmode_8_pixels = false, .enable_horizontal_interrupt = false, .lock_hv_counter = false});
			break;
	}

	++horizontal_interrupt_flip_flop;
}

// Runs once per frame, either 50 or 60 times a second for PAL or NTSC respectively.
void _VerticalInterruptHandler()
{
	if (!waiting_for_vertical_interrupt)
		return;

	waiting_for_vertical_interrupt = false;

	Controller::manager.Update();

	Z80::Bus z80_bus;
	Sprite::UploadTable(z80_bus);

	// Set vertical scroll value.
	VDP::SendCommand(VDP::RAM::VSRAM, VDP::Access::WRITE, 0);
	VDP::Write(VDP::DataValueWord(Level::camera.y));

	// Set horizontal scroll value.
	VDP::SendCommand(VDP::RAM::VRAM, VDP::Access::WRITE, 0xDC00);
	VDP::Write(VDP::DataValueWord(-Level::camera.x));

	Level::Draw(z80_bus);
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

static void DrawHUD()
{
	// Fill-in a region of Window Plane with tile data for the HUD.
	// I would have located the HUD on the left, like it was in the original TOFU's Assignment fangame,
	// but Window Plane is bugged on the Mega Drive, causing tiles to its right to be corrupted.
	// So, to work around this, Window Plane is only displayed on the right instead.

	constexpr auto ComputeWindowPlaneVRAMAddress = [](const Coordinate::Tile &position)
	{
		constexpr auto ComputePlaneVRAMAddress = [](const unsigned int plane_vram_address, const Coordinate::Tile &position)
		{
			return plane_vram_address + (position.y * Coordinate::plane_size.tiles.x + position.x) * sizeof(VDP::VRAM::TileMetadata);
		};

		return ComputePlaneVRAMAddress(0xF000, position);
	};

	// Draw background.
	for (unsigned int y = 0; y < Coordinate::hud_size.tiles.y; ++y)
	{
		VDP::SendCommand(VDP::RAM::VRAM, VDP::Access::WRITE, ComputeWindowPlaneVRAMAddress(Coordinate::Tile(Coordinate::screen_size.tiles.x - Coordinate::hud_size.tiles.x, y)));

		for (unsigned int x = 0; x < Coordinate::hud_size.tiles.x; ++x)
			VDP::Write(VDP::VRAM::TileMetadata{.priority = true, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 0x11});
	}

	// Draw text.
	VDP::SendCommand(VDP::RAM::VRAM, VDP::Access::WRITE, ComputeWindowPlaneVRAMAddress(Coordinate::Tile(Coordinate::screen_size.tiles.x - Coordinate::hud_size.tiles.x + 2, 1)));
	for (unsigned int i = 0; i < 8; ++i)
		VDP::Write(VDP::VRAM::TileMetadata{.priority = true, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 0x100 - ' ' + '0' + i});
}

static void WaitForVerticalInterrupt()
{
	waiting_for_vertical_interrupt = true;
	do
		asm("stop #0x2000");
	while (waiting_for_vertical_interrupt);
}

// Run indefinitely; should not return. Handles the bulk of operations.
void _EntryPoint()
{
	VDP::Register01 vdp_register_01{.enable_display = false, .enable_vertical_interrupt = true, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	VDP::Write(vdp_register_01);

	// Upload stuff to the VDP.
	{
		Z80::Bus z80_bus;

		static constexpr auto WordsFromBytes = []<std::size_t S>(const unsigned char (&bytes)[S]) constexpr
		{
			static_assert(S % 2 == 0, "Data must be an even number of bytes long!");
			std::array<unsigned short, S / 2> words;
			for (std::size_t i = 0; i < std::size(words); ++i)
				words[i] = static_cast<unsigned short>(bytes[i * 2 + 0]) << 8 | bytes[i * 2 + 1];
			return words;
		};

		static constexpr auto tiles = WordsFromBytes({
			#embed "assets/tiles.unc"
		});
		z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL * 4, std::data(tiles), std::size(tiles));

		static constexpr auto palette = WordsFromBytes({
			#embed "assets/palette.unc"
		});
		z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::CRAM, 0, std::data(palette), std::size(palette));

		static constexpr auto font = [&]() constexpr
		{
			unsigned char font[] = {
				#embed "../common/font.unc"
			};

			// Preprocess the font to recolour it. Modern C++ kicks ass!
			for (auto &byte : font)
			{
				auto nybbles = std::to_array({byte >> 4, byte & 0xF});

				for (auto &nybble : nybbles)
				{
					if (nybble == 0)
						nybble = 1;
					else if (nybble == 1)
						nybble = 7;
				}

				byte = nybbles[0] << 4 | nybbles[1];
			}

			return WordsFromBytes(font);
		}();
		z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL * 0x100, std::data(font), std::size(font));

		// Draw level.
		Level::DrawWholeScreen(z80_bus);
	}

	DrawHUD();

	vdp_register_01.enable_display = true;
	VDP::Write(vdp_register_01);

	// Spawn player.
	Objects::EmplaceFront<Objects::Player>(Coordinate::Block(7, 3));

	for (;;)
	{
		// Configure horizontal interrupt.
		VDP::SetHorizontalInterruptInterval(2);
		horizontal_interrupt_flip_flop = 0;

		// Enable Window Plane at the top of the screen.
		// We will disable it in the horizontal interrupt to end it mid-way through the screen.
		VDP::Write(VDP::Register00{.blank_leftmode_8_pixels = false, .enable_horizontal_interrupt = true, .lock_hv_counter = false});

		Objects::Update();
		WaitForVerticalInterrupt();
	}
}
