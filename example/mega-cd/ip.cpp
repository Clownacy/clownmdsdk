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

static std::array<unsigned short, 0x400> sector_buffer;

static constexpr unsigned int VRAM_PLANE_A = 0xC000;
static constexpr unsigned int PLANE_WIDTH = 64;

static void SetupPlaneWrite(const unsigned int x, unsigned int y)
{
	MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::WRITE, VRAM_PLANE_A + (y * PLANE_WIDTH + x) * 2);
}

static void DrawString(const char* const string, const unsigned int palette_line = 0)
{
	for (const char* character = string; *character != '\0'; ++character)
		MD::VDP::Write(MD::VDP::VRAM::TileMetadata{false, palette_line, false, false, static_cast<unsigned int>(*character)});
}

static void DrawHexViewer()
{
	static constexpr auto DrawHexWord = [](const unsigned short value, const unsigned int palette_line = 0)
	{
		static constexpr unsigned int BITS_PER_NYBBLE = 4;
		static constexpr unsigned int NYBBLES_PER_VALUE = 16 / BITS_PER_NYBBLE;
		static constexpr unsigned int NYBBLE_MASK = (1 << BITS_PER_NYBBLE) - 1;

		for (unsigned int i = 0; i < NYBBLES_PER_VALUE; ++i)
		{
			static constexpr auto NybbleToASCII = [](const unsigned int nybble)
			{
				if (nybble >= 0xA)
					return nybble - 0xA + 'A';
				else
					return nybble - 0x0 + '0';
			};

			MD::VDP::Write(MD::VDP::VRAM::TileMetadata{false, palette_line, false, false, NybbleToASCII(value << i * BITS_PER_NYBBLE >> ((NYBBLES_PER_VALUE - 1) * BITS_PER_NYBBLE) & NYBBLE_MASK)});
		}
	};

	auto sector_buffer_pointer = sector_buffer.data();

	for (unsigned int y = 0; y < 224 / 8 - 3; ++y)
	{
		SetupPlaneWrite(2, 2 + y);
		DrawHexWord(y * 0x10, 2);

		bool flipflop = false;

		for (unsigned int x = 0; x < 8; ++x)
		{
			DrawHexWord(*sector_buffer_pointer++, flipflop);

			flipflop = !flipflop;
		}
	}
}

__attribute__((interrupt)) static void VerticalInterrupt()
{
	MD::MegaCD::subcpu.raise_interrupt_level_2 = true;
}

void _EntryPoint()
{
	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, 0);
	for (unsigned int i = 0; i < 64; ++i)
		MD::VDP::Write(MD::VDP::CRAM::Colour{7, 0, 0});

	MD::MegaCD::jump_table.level_6.address = VerticalInterrupt;

	MD::MegaCD::communication_flag_ours = 0x87;
	while (MD::MegaCD::communication_flag_theirs != 0x87);

	for (unsigned int i = 0; i < 64; ++i)
		MD::VDP::Write(MD::VDP::CRAM::Colour{0, 7, 0});

	auto vdp_register01 = MD::VDP::Register01{.enable_display = true, .enable_vertical_interrupt = false, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	MD::VDP::Write(vdp_register01);

	MD::VDP::VRAM::SetPlaneALocation(VRAM_PLANE_A);

//	MD::MegaCD::subcpu.bus_request = true;

	while (MD::MegaCD::communication_flag_theirs != 0x97);

	for (unsigned int i = 0; i < 64; ++i)
		MD::VDP::Write(MD::VDP::CRAM::Colour{0, 0, 7});

	unsigned short *sector_buffer_pointer = sector_buffer.data();

	while (!MD::MegaCD::cdc_mode.data_set_ready);

	// Read header junk.
	*sector_buffer_pointer = MD::MegaCD::cdc_host_data;
	*sector_buffer_pointer = MD::MegaCD::cdc_host_data;

	while (!MD::MegaCD::cdc_mode.end_of_data_transfer)
		*sector_buffer_pointer++ = MD::MegaCD::cdc_host_data;

	MD::MegaCD::communication_flag_ours = 0x97;

	// Write colours to CRAM.
	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, 0);
	MD::VDP::Write(MD::VDP::CRAM::Colour{0, 0, 0});
	MD::VDP::Write(MD::VDP::CRAM::Colour{7, 7, 7});

	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, (16 + 1) * 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{5, 5, 5});

	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, (32 + 1) * 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{2, 2, 7});

	SetupPlaneWrite(2, 1);
	DrawString("Test");

	DrawHexViewer();

	for (;;)
	{
	}
}
