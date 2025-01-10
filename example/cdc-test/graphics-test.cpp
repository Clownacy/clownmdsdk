#include "graphics-test.h"

#include "command.h"
#include "control-pad.h"
#include "system.h"
#include "utility.h"

GraphicsTest::GraphicsTest()
{
	const unsigned int width = 7;
	const unsigned int height = 3;

	ClearPlaneA();
	for (unsigned int y = 0; y < height; ++y)
	{
		MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::WRITE, VRAM_PLANE_A + y * PLANE_WIDTH * 2);
		for (unsigned int x = 0; x < width; ++x)
			MD::VDP::Write(MD::VDP::DataValueWord(0x100 + x * height + y));
	}

	SubmitSubCPUCommand(Command::REQUEST_WORD_RAM);

	// Copy stamps.
	MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::READ, ' ' * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL);

	for (unsigned int i = 0; i < 0x100 * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL / 2; ++i)
	{
		MCD_RAM::word_ram_2m<unsigned short>[0x100 + i] = MD::VDP::data_port_word;
	}

	MCD_RAM::GiveWordRAMToSubCPU();
	SubmitSubCPUCommand(Command::DO_GRAPHICS_TRANSFORMATION);
	SubmitSubCPUCommand(Command::REQUEST_WORD_RAM);

	const auto total_words = width * height * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL / 2;
	MD::Z80::Bus z80_bus;
	z80_bus.CopyWordsToVDPWithDMA(MD::VDP::RAM::VRAM, 0x100 * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL, &MCD_RAM::word_ram_2m<unsigned short>[0x20000 / 2 + 1], total_words);
	MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::WRITE, 0x100 * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL);
	MD::VDP::Write(MD::VDP::DataValueWord(MCD_RAM::word_ram_2m<unsigned short>[0x20000 / 2]));
}

ModeID GraphicsTest::Update()
{
	if (control_pads[0].pressed.b)
		return ModeID::MAIN_MENU;

	return ModeID::UNCHANGED;
}
