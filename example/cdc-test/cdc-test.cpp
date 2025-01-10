#include "cdc-test.h"

#include <array>
#include <span>
#include <utility>

#include "command.h"
#include "control-pad.h"
#include "system.h"
#include "utility.h"

struct TransferMode
{
	const char *label;
	Command command;
};

static const auto transfer_modes = std::to_array<TransferMode>({
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
	{"DMA PRG-RAM (Offset 8)"   , Command::BEGIN_TRANSFER_DMA_PRG_OFFSET_8       },
	{"DMA Word-RAM"             , Command::BEGIN_TRANSFER_DMA_WORD               },
	{"DMA Word-RAM (Offset 8)"  , Command::BEGIN_TRANSFER_DMA_WORD_OFFSET_8      },
});

static auto &sector_buffer = *reinterpret_cast<std::array<unsigned short, 0x408>*>(MCD_RAM::word_ram_2m<unsigned short>.data());

static constexpr unsigned int HEX_VIEWER_TOTAL_WORDS_PER_ROW = 8;
static constexpr unsigned int HEX_VIEWER_TOTAL_ROWS = 224 / 8 - 2 - 1;
static constexpr unsigned int HEX_VIEWER_TOTAL_WORDS_VISIBLE = HEX_VIEWER_TOTAL_WORDS_PER_ROW * HEX_VIEWER_TOTAL_ROWS;

static void DoTransfer(const Command command)
{
	std::fill(std::begin(sector_buffer), std::end(sector_buffer), 0);

	if (command == Command::NONE)
		return;

	MCD_RAM::GiveWordRAMToSubCPU();

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

void CDCTest::DrawHexViewer()
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

void CDCTest::DoEverything()
{
	static constexpr auto DrawHeader = [](const char* const string)
	{
		SetupPlaneWrite(10, 1);
		DrawString(string);
	};

	DrawHeader("                         ");
	DrawHeader(transfer_modes[current_mode].label);
	DoTransfer(transfer_modes[current_mode].command);
	DrawHexViewer();
};

CDCTest::CDCTest()
{
	MD::VDP::VRAM::SetPlaneALocation(VRAM_PLANE_A);

	SetupPlaneWrite(2, 1);
	DrawString("Method: ");

	SubmitSubCPUCommand(Command::REQUEST_WORD_RAM);

	DoEverything();
}

ModeID CDCTest::Update()
{
	// Scroll up and down.
	const auto old_hex_viewer_starting_position = hex_viewer_starting_position;
	static constexpr unsigned int scroll_amount = 8;

	if (control_pads[0].pressed.b)
		return ModeID::MAIN_MENU;

	if (control_pads[0].held.up)
		hex_viewer_starting_position -= std::min<unsigned int>(scroll_amount, hex_viewer_starting_position);
	if (control_pads[0].held.down)
		hex_viewer_starting_position += std::min<unsigned int>(scroll_amount, std::size(sector_buffer) - HEX_VIEWER_TOTAL_WORDS_VISIBLE - hex_viewer_starting_position);

	if (old_hex_viewer_starting_position != hex_viewer_starting_position)
		DrawHexViewer();

	// Cycle methods.
	const auto old_current_mode = current_mode;

	if (control_pads[0].pressed.left)
	{
		if (current_mode == 0)
			current_mode = std::size(transfer_modes) - 1;
		else
			--current_mode;
	}
	if (control_pads[0].pressed.right)
	{
		if (current_mode == std::size(transfer_modes) - 1)
			current_mode = 0;
		else
			++current_mode;
	}

	if (old_current_mode != current_mode)
		DoEverything();

	return ModeID::UNCHANGED;
}
