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

#include <atomic>
#include <span>
#include <utility>

#include <clownmdsdk.h>

#include "../common/control-pad-manager.h"

#include "command.h"

namespace MD = ClownMDSDK::MainCPU;

struct Mode
{
	const char *label;
	Command command;
};

static const auto modes = std::to_array<Mode>({
	{"None"                     , Command::NONE                                  },
	{"BIOS CDCTRN"              , Command::BEGIN_TRANSFER_BIOS                   },
	{"Host Main-CPU (Past End)" , Command::BEGIN_TRANSFER_HOST_MAIN_READ_PAST_END},
	{"Host Main-CPU (Until DSR)", Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_DSR },
	{"Host Main-CPU (Until EDT)", Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_EDT },
	{"Host Sub-CPU (Past End)"  , Command::BEGIN_TRANSFER_HOST_SUB_READ_PAST_END },
	{"Host Sub-CPU (Until DSR)" , Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_DSR  },
	{"Host Sub-CPU (Until EDT)" , Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_EDT  },
	{"DMA WAVE-RAM"             , Command::BEGIN_TRANSFER_DMA_PCM                },
	{"DMA WAVE-RAM (Offset 8)"  , Command::BEGIN_TRANSFER_DMA_PCM_OFFSET_8       },
	{"DMA PRG-RAM"              , Command::BEGIN_TRANSFER_DMA_PRG                },
	{"DMA Word-RAM"             , Command::BEGIN_TRANSFER_DMA_WORD               },
	{"DMA Word-RAM (Offset 8)"  , Command::BEGIN_TRANSFER_DMA_WORD_OFFSET_8      },
});

static constexpr unsigned int VRAM_PLANE_A = 0xC000;
static constexpr unsigned int PLANE_WIDTH = 64;

static ControlPadManager<1> control_pad_manager;
static const auto &control_pads = control_pad_manager.GetControlPads();

static unsigned int hex_viewer_starting_position = 0;
static const std::span<unsigned short, 0x408> sector_buffer(MD::MegaCD::word_ram_2m<unsigned short>.data(), 0x408);
static unsigned int current_mode = 0;

static void SetupPlaneWrite(const unsigned int x, unsigned int y)
{
	MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::WRITE, VRAM_PLANE_A + (y * PLANE_WIDTH + x) * 2);
}

static void DrawString(const char* const string, const unsigned int palette_line = 0)
{
	for (const char* character = string; *character != '\0'; ++character)
		MD::VDP::Write(MD::VDP::VRAM::TileMetadata{false, palette_line, false, false, static_cast<unsigned int>(*character)});
}

static constexpr unsigned int HEX_VIEWER_TOTAL_WORDS_PER_ROW = 8;
static constexpr unsigned int HEX_VIEWER_TOTAL_ROWS = 224 / 8 - 2 - 1;
static constexpr unsigned int HEX_VIEWER_TOTAL_WORDS_VISIBLE = HEX_VIEWER_TOTAL_WORDS_PER_ROW * HEX_VIEWER_TOTAL_ROWS;

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

	auto sector_buffer_pointer = sector_buffer.data() + hex_viewer_starting_position;

	for (unsigned int y = 0; y < HEX_VIEWER_TOTAL_ROWS; ++y)
	{
		SetupPlaneWrite(2, 2 + y);
		DrawHexWord((hex_viewer_starting_position + y * HEX_VIEWER_TOTAL_WORDS_PER_ROW) * 2, 2);

		bool flipflop = false;

		for (unsigned int x = 0; x < HEX_VIEWER_TOTAL_WORDS_PER_ROW; ++x)
		{
			DrawHexWord(*sector_buffer_pointer++, flipflop);

			flipflop = !flipflop;
		}
	}
}

__attribute__((interrupt)) static void VerticalInterrupt()
{
	MD::MegaCD::subcpu.raise_interrupt_level_2 = true;

	control_pad_manager.Update();
}

static void SubmitSubCPUCommand(const Command command)
{
	static constexpr auto Internal = [](const Command command)
	{
		MD::MegaCD::communication_flag_ours = std::to_underlying(command);
		while (MD::MegaCD::communication_flag_theirs != std::to_underlying(command));
	};

	Internal(command);
	Internal(Command::NONE);
}

static void DoTransfer(const Command command)
{
	std::fill(std::begin(sector_buffer), std::end(sector_buffer), 0);

	if (command == Command::NONE)
		return;

	MD::MegaCD::GiveWordRAMToSubCPU();

	// Start transfer.
	SubmitSubCPUCommand(command);

	switch (command)
	{
		default:
			break;

		case Command::BEGIN_TRANSFER_HOST_MAIN_READ_PAST_END:
		case Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_DSR:
		case Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_EDT:
			while (!MD::MegaCD::CDC::mode.data_set_ready);

			auto *sector_buffer_pointer = sector_buffer.data();

			// Read header junk.
			*sector_buffer_pointer = MD::MegaCD::CDC::host_data;
			*sector_buffer_pointer = MD::MegaCD::CDC::host_data;

			// Read actual sector data.
			switch (command)
			{
				default:
					break;

				case Command::BEGIN_TRANSFER_HOST_MAIN_READ_PAST_END:
					for (auto &word : sector_buffer)
						word = MD::MegaCD::CDC::host_data;
					break;

				case Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_DSR:
					while (MD::MegaCD::CDC::mode.data_set_ready)
						*sector_buffer_pointer++ = MD::MegaCD::CDC::host_data;
					break;

				case Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_EDT:
					while (!MD::MegaCD::CDC::mode.end_of_data_transfer)
						*sector_buffer_pointer++ = MD::MegaCD::CDC::host_data;
					break;
			}

			break;
	}

	// Finish transfer.
	SubmitSubCPUCommand(Command::END_TRANSFER);
}

void _EntryPoint()
{
	auto vdp_register01 = MD::VDP::Register01{.enable_display = false, .enable_vertical_interrupt = false, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	MD::VDP::Write(vdp_register01);

	MD::MegaCD::jump_table.level_6.address = VerticalInterrupt;

	MD::VDP::VRAM::SetPlaneALocation(VRAM_PLANE_A);

	// Write colours to CRAM.
	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, 0);
	MD::VDP::Write(MD::VDP::CRAM::Colour{0, 0, 0});
	MD::VDP::Write(MD::VDP::CRAM::Colour{7, 7, 7});

	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, (16 + 1) * 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{5, 5, 5});

	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, (32 + 1) * 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{2, 2, 7});

	vdp_register01.enable_display = true;
	vdp_register01.enable_vertical_interrupt = true;
	MD::VDP::Write(vdp_register01);

	SetupPlaneWrite(2, 1);
	DrawString("Method: ");

	static constexpr auto DoEverything = []()
	{
		static constexpr auto DrawHeader = [](const char* const string)
		{
			SetupPlaneWrite(10, 1);
			DrawString(string);
		};

		DrawHeader("                         ");
		DrawHeader(modes[current_mode].label);
		DoTransfer(modes[current_mode].command);
		DrawHexViewer();
	};

	DoEverything();

	for (;;)
	{
		// Wait for vertical interrupt.
		asm("stop #0x2000");

		const auto old_hex_viewer_starting_position = hex_viewer_starting_position;
		static constexpr unsigned int scroll_amount = 8;

		if (control_pads[0].held.up)
			hex_viewer_starting_position -= std::min<unsigned int>(scroll_amount, hex_viewer_starting_position);
		if (control_pads[0].held.down)
			hex_viewer_starting_position += std::min<unsigned int>(scroll_amount, std::size(sector_buffer) - HEX_VIEWER_TOTAL_WORDS_VISIBLE - hex_viewer_starting_position);

		if (old_hex_viewer_starting_position != hex_viewer_starting_position)
			DrawHexViewer();

		const auto old_current_mode = current_mode;

		if (control_pads[0].pressed.left)
		{
			if (current_mode == 0)
				current_mode = std::size(modes) - 1;
			else
				--current_mode;
		}
		if (control_pads[0].pressed.right)
		{
			if (current_mode == std::size(modes) - 1)
				current_mode = 0;
			else
				++current_mode;
		}

		if (old_current_mode != current_mode)
			DoEverything();
	}
}
