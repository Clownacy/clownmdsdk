// Copyright (c) 2024-2026 Clownacy

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
#include <cstdlib>

#include <clownmdsdk.h>

#include "../common/control-pad-manager.h"

using namespace ClownMDSDK::MainCPU;

static constexpr unsigned int SCREEN_WIDTH = 320;
static constexpr unsigned int SCREEN_HEIGHT = 224 * 2;

static std::atomic<bool> waiting_for_v_int;
static ControlPadManager<2> control_pad_manager;

static constexpr unsigned int VRAM_HSCROLL = 0x7C00;
static constexpr unsigned int VRAM_PLANE_A = 0x8000;
static constexpr unsigned int VRAM_WINDOW_PLANE = 0xA000;
static constexpr unsigned int VRAM_PLANE_B = 0xC000;
static constexpr unsigned int VRAM_SPRITE_TABLE = 0xD800;

static VDP::Register01 vdp_register01;

static unsigned int camera_x, camera_y;

static int window_x_boundary = 2, window_y_boundary = 2;

static void WaitForVInt()
{
	waiting_for_v_int = true;

	do
	{
		asm("stop #0x2000");
	} while (waiting_for_v_int);
}

static void UpdateWindowBoundaries()
{
	static constexpr auto horizontal_limit = SCREEN_WIDTH / (VDP::VRAM::TILE_WIDTH * 2);
	static constexpr auto vertical_limit = SCREEN_HEIGHT / VDP::VRAM::TILE_HEIGHT_INTERLACE_MODE_2;

	if (window_x_boundary < 0)
		VDP::Write(VDP::Register11{.window_align_right = true, .window_width = static_cast<unsigned int>(horizontal_limit + window_x_boundary)});
	else
		VDP::Write(VDP::Register11{.window_align_right = false, .window_width = static_cast<unsigned int>(window_x_boundary)});

	if (window_y_boundary < 0)
		VDP::Write(VDP::Register12{.window_align_bottom = true, .window_height = static_cast<unsigned int>(vertical_limit + window_y_boundary)});
	else
		VDP::Write(VDP::Register12{.window_align_bottom = false, .window_height = static_cast<unsigned int>(window_y_boundary)});
}

void _EntryPoint()
{
	vdp_register01 = {.enable_display = false, .enable_vertical_interrupt = false, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	VDP::Write(vdp_register01);
	VDP::Write(VDP::Register0C{.enable_h40_cell_mode_1 = true, .enable_shadow_highlight_mode = false, .interlace_mode = 3, .enable_h40_cell_mode_2 = true});
	VDP::VRAM::SetHorizontalScrollLocation(VRAM_HSCROLL);
	VDP::VRAM::SetPlaneALocation(VRAM_PLANE_A);
	VDP::VRAM::SetWindowPlaneLocation(VRAM_WINDOW_PLANE);
	VDP::VRAM::SetPlaneBLocation(VRAM_PLANE_B);
	VDP::VRAM::SetSpriteTableLocation(VRAM_SPRITE_TABLE);
	UpdateWindowBoundaries();

	// Write colours to CRAM.
	for (unsigned int i = 0; i < 4; ++i)
		for (unsigned int j = 1; j < 8; ++j)
			VDP::CRAM::Set(i, j, VDP::CRAM::Colour(i == 0 || i == 1 ? j : 0, i == 0 || i == 2 ? j : 0, i == 0 || i == 3 ? j : 0));

	// Set up the tiles.
	Z80::Bus::Lock(
		[&](auto &z80_bus)
		{
			const auto DoTile = [&z80_bus](const unsigned int tile_index, const unsigned int colour)
			{
				std::array<unsigned short, VDP::VRAM::TILE_SIZE_IN_BYTES_INTERLACE_MODE_2 / sizeof(short)> tile_buffer;
				unsigned short *tile_buffer_pointer = std::data(tile_buffer);

				for (unsigned int y = 0; y < VDP::VRAM::TILE_HEIGHT_INTERLACE_MODE_2; ++y)
				{
					unsigned int word = 0;

					for (unsigned int x = 0; x < VDP::VRAM::TILE_WIDTH; ++x)
					{
						const unsigned int x_distance = std::abs(x - VDP::VRAM::TILE_WIDTH / 2);
						const unsigned int y_distance = std::abs(y - VDP::VRAM::TILE_HEIGHT_INTERLACE_MODE_2 / 2);

						word <<= 4;
						word |= x_distance < VDP::VRAM::TILE_WIDTH / 8 && y_distance < VDP::VRAM::TILE_HEIGHT_INTERLACE_MODE_2 / 8 ? colour : 0;

						if (x % 4 == 3)
							*tile_buffer_pointer++ = word;
					}
				}

				z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, tile_index * VDP::VRAM::TILE_SIZE_IN_BYTES_INTERLACE_MODE_2, std::data(tile_buffer), std::size(tile_buffer));
			};

			DoTile(1, 1);
			DoTile(2, 2);
			DoTile(3, 3);
			DoTile(4, 4);
		}
	);

//	VDP::SendCommand(VDP::RAM::VRAM, VDP::Access::WRITE, VDP::VRAM::TILE_SIZE_IN_BYTES_INTERLACE_MODE_2 * 1);

	// Set up the planes.
	const auto FillPlane = [](const unsigned int vram_address, const unsigned int palette_line)
	{
		VDP::SendCommand(VDP::RAM::VRAM, VDP::Access::WRITE, vram_address);

		for (unsigned int y = 0; y < 32; ++y)
		{
			for (unsigned int x = 0; x < 64; ++x)
			{
				VDP::Write(VDP::VRAM::TileMetadata{false, palette_line, false, false, 1 + ((x & 3) ^ (y & 3))});
			}
		}
	};

	FillPlane(VRAM_PLANE_A,      1);
	FillPlane(VRAM_WINDOW_PLANE, 2);
	FillPlane(VRAM_PLANE_B,      3);

	// Now that initialisation is complete, enable display.
	vdp_register01.enable_display = true;
	vdp_register01.enable_vertical_interrupt = true;
	VDP::Write(vdp_register01);

	M68k::SetInterruptMask(0);

	for (;;)
	{
		WaitForVInt();

		const auto &control_pad = control_pad_manager.GetControlPad(0);

		if (control_pad.held.c)
		{
			if (control_pad.pressed.left)
				--window_x_boundary;
			if (control_pad.pressed.right)
				++window_x_boundary;
			if (control_pad.pressed.up)
				--window_y_boundary;
			if (control_pad.pressed.down)
				++window_y_boundary;
		}
		else
		{
			if (control_pad.held.left)
				--camera_x;
			if (control_pad.held.right)
				++camera_x;
			if (control_pad.held.up)
				--camera_y;
			if (control_pad.held.down)
				++camera_y;
		}
	}
}

[[noreturn]] static void ErrorTrap()
{
	VDP::CRAM::Fill({7, 0, 0});

	for (;;)
		asm("stop #0x2700");
}

void _BusErrorHandler()
{
	ErrorTrap();
}

void _AddressErrorHandler()
{
	ErrorTrap();
}

void _IllegalInstructionHandler()
{
	ErrorTrap();
}

void _DivisionByZeroHandler()
{
	ErrorTrap();
}

void _CHKHandler()
{
	ErrorTrap();
}

void _TRAPVHandler()
{
	ErrorTrap();
}

void _PrivilegeViolationHandler()
{
	ErrorTrap();
}

void _TraceHandler()
{
	ErrorTrap();
}

void _UnimplementedInstructionLineAHandler()
{
	ErrorTrap();
}

void _UnimplementedInstructionLineFHandler()
{
	ErrorTrap();
}

void _UnassignedHandler()
{
	ErrorTrap();
}

void _UninitialisedInterruptHandler()
{
	ErrorTrap();
}

void _SpuriousInterruptHandler()
{
	ErrorTrap();
}

void _Level1InterruptHandler()
{

}

void _ControllerInterruptHandler()
{

}

void _Level3InterruptHandler()
{

}

void _HorizontalInterruptHandler()
{

}

void _Level5InterruptHandler()
{

}

void _VerticalInterruptHandler()
{
	if (!waiting_for_v_int)
		return;

	waiting_for_v_int = false;

	control_pad_manager.Update();

	VDP::SendCommand(VDP::RAM::VRAM, VDP::Access::WRITE, VRAM_HSCROLL);
	VDP::Write(VDP::DataValueLongword(camera_x / 2, -(camera_x / 3)));
	VDP::SendCommand(VDP::RAM::VSRAM, VDP::Access::WRITE, 0);
	VDP::Write(VDP::DataValueLongword(-(camera_y / 2), camera_y / 3));

	UpdateWindowBoundaries();
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
