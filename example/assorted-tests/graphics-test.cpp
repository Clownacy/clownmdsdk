#include "graphics-test.h"

#include "command.h"
#include "control-pad.h"
#include "system.h"
#include "utility.h"

// Tests various edgecases of the Mega CD's graphics transformation hardware; useful for emulator development.
GraphicsTest::GraphicsTest()
{
	const unsigned int width = 7;
	const unsigned int height = 3;

	// Initialise Plane A, for displaying the transformed graphics.
	ClearPlaneA();
	for (unsigned int y = 0; y < height; ++y)
	{
		MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::WRITE, VRAM_PLANE_A + y * PLANE_WIDTH * 2);
		for (unsigned int x = 0; x < width; ++x)
			MD::VDP::Write(MD::VDP::VRAM::TileMetadata{
				.priority = false,
				.palette_line = 0,
				.y_flip = false,
				.x_flip = false,
				.tile_index = 0x100 + x * height + y
			});
	}

	// Copy ASCII tiles from VRAM to Word-RAM to serve as the stamp.
	SubmitSubCPUCommand(Command::REQUEST_WORD_RAM);
	MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::READ, ' ' * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL);
	for (unsigned int i = 0; i < 0x100 * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL / 2; ++i)
		MCD_RAM::word_ram_2m<unsigned short>[0x100 + i] = MD::VDP::data_port_word;

	// Transform the graphics.
	MCD_RAM::GiveWordRAMToSubCPU();
	SubmitSubCPUCommand(Command::DO_GRAPHICS_TRANSFORMATION);

	// Send the transformed graphics to VRAM, for display.
	SubmitSubCPUCommand(Command::REQUEST_WORD_RAM);
	MD::Z80::Bus::Lock(
		[&](auto &z80_bus)
		{
			z80_bus.CopyWordsToVDPWithDMAFromWordRAM(
				MD::VDP::RAM::VRAM,
				0x100 * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL,
				&MCD_RAM::word_ram_2m<unsigned short>[0x20000 / 2],
				width * height * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL / 2
			);
		}
	);
}

ModeID GraphicsTest::Update()
{
	if (control_pads[0].pressed.b)
		return ModeID::MAIN_MENU;

	return ModeID::UNCHANGED;
}
