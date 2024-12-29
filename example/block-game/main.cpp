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

#include <array>
#include <atomic>

#include <clownmdsdk.h>

#include "../common/joypad-manager.h"

namespace MD = ClownMDSDK::MainCPU;

static constexpr unsigned int SCREEN_WIDTH = 320;
static constexpr unsigned int SCREEN_HEIGHT = 224;

static unsigned int RandomNumber();

struct Puyo
{
	enum class Colour : unsigned char
	{
		BLUE,
		GREEN,
		YELLOW,
		RED,
		PURPLE
	};

	unsigned int x, y;
	Colour colour;

	static Puyo::Colour RandomColour()
	{
		static constexpr std::array colours = {
			Colour::BLUE,
			Colour::GREEN,
			Colour::YELLOW,
			Colour::RED,
			Colour::PURPLE
		};

		return colours[RandomNumber() % static_cast<unsigned int>(colours.size())];
	}

	static constexpr unsigned int Width()  {return 16;}
	static constexpr unsigned int Height() {return 16;}
	unsigned int Left()   const {return x;}
	unsigned int Right()  const {return x + Width();}
	unsigned int Top()    const {return y;}
	unsigned int Bottom() const {return y + Height();}
};

enum class GridSlot : unsigned char
{
	NOTHING,
	BLUE,
	GREEN,
	YELLOW,
	RED,
	PURPLE
};

static std::atomic<bool> waiting_for_v_int;

static JoypadManager<1> joypad_manager;
static const auto &joypads = joypad_manager.GetJoypads();

static constexpr unsigned int GRID_WIDTH_IN_PUYOS = 6;
static constexpr unsigned int GRID_HEIGHT_IN_PUYOS = 12;

static constexpr unsigned int LEFT_GRID_X_IN_TILES = 2;
static constexpr unsigned int RIGHT_GRID_X_IN_TILES = 26;
static constexpr unsigned int GRID_Y_IN_TILES = 2;
static constexpr unsigned int GRID_WIDTH_IN_TILES = GRID_WIDTH_IN_PUYOS * Puyo::Width() / MD::VDP::VRAM::TILE_WIDTH;
static constexpr unsigned int GRID_HEIGHT_IN_TILES = GRID_HEIGHT_IN_PUYOS * Puyo::Height() / MD::VDP::VRAM::TILE_HEIGHT_NORMAL;

static constexpr unsigned int LEFT_GRID_X = LEFT_GRID_X_IN_TILES * MD::VDP::VRAM::TILE_WIDTH;
static constexpr unsigned int RIGHT_GRID_X = RIGHT_GRID_X_IN_TILES * MD::VDP::VRAM::TILE_WIDTH;
static constexpr unsigned int GRID_Y = GRID_Y_IN_TILES * MD::VDP::VRAM::TILE_HEIGHT_NORMAL;
static constexpr unsigned int GRID_WIDTH = GRID_WIDTH_IN_TILES * MD::VDP::VRAM::TILE_WIDTH;
static constexpr unsigned int GRID_HEIGHT = GRID_HEIGHT_IN_TILES * MD::VDP::VRAM::TILE_HEIGHT_NORMAL;

static constexpr unsigned int VRAM_PLANE_A = 0xC000;
static constexpr unsigned int VRAM_SPRITE_TABLE = 0xD800;

static Puyo puyo{.x = 0, .y = 0, .colour = Puyo::RandomColour()};
static std::array<std::array<GridSlot, GRID_WIDTH_IN_PUYOS>, GRID_HEIGHT_IN_PUYOS> left_grid;
static MD::VDP::Register01 vdp_register01;

static unsigned int RandomNumber()
{
	// https://en.wikipedia.org/wiki/Xorshift#Example_implementation
	static unsigned long state = 0x55555555;
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

static constexpr GridSlot PuyoColourToGridSlot(const Puyo::Colour colour)
{
	switch (colour)
	{
		default:
		case Puyo::Colour::BLUE:
			return GridSlot::BLUE;

		case Puyo::Colour::GREEN:
			return GridSlot::GREEN;

		case Puyo::Colour::YELLOW:
			return GridSlot::YELLOW;

		case Puyo::Colour::RED:
			return GridSlot::RED;

		case Puyo::Colour::PURPLE:
			return GridSlot::PURPLE;
	}
};

static constexpr unsigned int GridSlotToTileIndex(const GridSlot grid_slot)
{
	switch (grid_slot)
	{
		default:
		case GridSlot::NOTHING:
			return 2;

		case GridSlot::BLUE:
			return 3 + 4 * 0;

		case GridSlot::GREEN:
			return 3 + 4 * 1;

		case GridSlot::YELLOW:
			return 3 + 4 * 2;

		case GridSlot::RED:
			return 3 + 4 * 3;

		case GridSlot::PURPLE:
			return 3 + 4 * 4;
	}
};

static void WaitForVInt()
{
	waiting_for_v_int = true;

	do
	{
		asm("stop #0x2000");
	} while (waiting_for_v_int);
}

void _EntryPoint()
{
	vdp_register01 = {.enable_display = false, .enable_vertical_interrupt = false, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	MD::VDP::Write(vdp_register01);
	MD::VDP::VRAM::SetPlaneALocation(VRAM_PLANE_A);
	MD::VDP::VRAM::SetSpriteTableLocation(VRAM_SPRITE_TABLE);

	// Write colours to CRAM.
	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{0, 0, 0}, MD::VDP::CRAM::Colour{1, 1, 1});
	MD::VDP::Write(MD::VDP::CRAM::Colour{0, 3, 7}, MD::VDP::CRAM::Colour{0, 7, 0});
	MD::VDP::Write(MD::VDP::CRAM::Colour{7, 7, 0}, MD::VDP::CRAM::Colour{7, 0, 0});
	MD::VDP::Write(MD::VDP::CRAM::Colour{4, 0, 7});

	const auto FillTiles = [](const unsigned int colour_index, const unsigned int tile_index, const unsigned int total_tiles) __attribute__((always_inline))
	{
		MD::VDP::VRAM::FillBytesWithDMA((tile_index) * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL, (total_tiles) * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL, MD::VDP::RepeatBits<unsigned int, 2>(colour_index));
		MD::VDP::WaitUntilDMAIsComplete();
	};

	const auto FillTile = [&FillTiles](const unsigned int colour_index, const unsigned int tile_index) __attribute__((always_inline))
	{
		FillTiles(colour_index, tile_index, 1);
	};

	// Use DMA Fill to generate some coloured tiles.
	MD::VDP::SetAddressIncrement(1);
	FillTile(1, 1);
	FillTile(2, 2);
	FillTiles(3, GridSlotToTileIndex(GridSlot::BLUE),   4);
	FillTiles(4, GridSlotToTileIndex(GridSlot::GREEN),  4);
	FillTiles(5, GridSlotToTileIndex(GridSlot::YELLOW), 4);
	FillTiles(6, GridSlotToTileIndex(GridSlot::RED),    4);
	FillTiles(7, GridSlotToTileIndex(GridSlot::PURPLE), 4);
	MD::VDP::SetAddressIncrement(2);

	static constexpr auto Plane64XYToOffset = [](const unsigned int x, const unsigned int y) constexpr
	{
		return (y * 64 + x) * sizeof(MD::VDP::VRAM::TileMetadata);
	};

	static constexpr auto DrawBox = [](const unsigned int offset_x, const unsigned int offset_y, const unsigned int width, const unsigned int height, const MD::VDP::VRAM::TileMetadata &tile_metadata)
	{
		for (unsigned int y = 0; y < height; ++y)
		{
			MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::WRITE, VRAM_PLANE_A + Plane64XYToOffset(offset_x, offset_y + y));

			for (unsigned int x = 0; x < width / 2; ++x)
				MD::VDP::Write(MD::VDP::DataValueLongword(MD::VDP::RepeatBits<unsigned long, 4>(std::bit_cast<unsigned short>(tile_metadata))));

			if (width % 2 != 0)
				MD::VDP::Write(MD::VDP::DataValueWord(std::bit_cast<unsigned short>(tile_metadata)));
		}
	};

	// Fill background.
	DrawBox(0, 0, SCREEN_WIDTH / MD::VDP::VRAM::TILE_WIDTH, SCREEN_HEIGHT / MD::VDP::VRAM::TILE_HEIGHT_NORMAL, MD::VDP::VRAM::TileMetadata{.priority = true, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 1});

	// Fill left box.
	DrawBox(LEFT_GRID_X_IN_TILES, GRID_Y_IN_TILES, GRID_WIDTH_IN_TILES, GRID_HEIGHT_IN_TILES, MD::VDP::VRAM::TileMetadata{.priority = false, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 2});

	// Fill right box.
	DrawBox(RIGHT_GRID_X_IN_TILES, GRID_Y_IN_TILES, GRID_WIDTH_IN_TILES, GRID_HEIGHT_IN_TILES, MD::VDP::VRAM::TileMetadata{.priority = false, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 2});

	// Now that initialisation is complete, enable display.
	vdp_register01.enable_display = true;
	vdp_register01.enable_vertical_interrupt = true;
	MD::VDP::Write(vdp_register01);

	MD::M68k::SetInterruptMask(0);

	for (;;)
	{
		WaitForVInt();

		puyo.y += joypads[0].held.down ? 3 : 1;

		if (puyo.Bottom() >= GRID_HEIGHT || left_grid[puyo.Bottom() / puyo.Height()][puyo.x / puyo.Width()] != GridSlot::NOTHING)
		{
			left_grid[puyo.y / puyo.Height()][puyo.x / puyo.Width()] = PuyoColourToGridSlot(puyo.colour);

			static constexpr auto DrawGridSlot = [](const unsigned int x, const unsigned int y)
			{
				const GridSlot grid_slot = left_grid[y][x];
				const auto tile_index = GridSlotToTileIndex(grid_slot);

				const auto puyo_height_in_tiles = Puyo::Height() / MD::VDP::VRAM::TILE_HEIGHT_NORMAL;

				for (unsigned int tile_y = 0; tile_y < puyo_height_in_tiles; ++tile_y)
				{
					const auto puyo_width_in_tiles = Puyo::Width() / MD::VDP::VRAM::TILE_WIDTH;

					MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::WRITE, VRAM_PLANE_A + Plane64XYToOffset(LEFT_GRID_X_IN_TILES + x * puyo_width_in_tiles, GRID_Y_IN_TILES + y * puyo_height_in_tiles + tile_y));

					for (unsigned int tile_x = 0; tile_x < puyo_width_in_tiles; ++tile_x)
					{
						const auto offset = grid_slot == GridSlot::NOTHING ? 0 : tile_x * puyo_height_in_tiles + tile_y;
						MD::VDP::Write(MD::VDP::DataValueWord(std::bit_cast<unsigned short>(MD::VDP::VRAM::TileMetadata{.priority = false, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = tile_index + offset})));
					}
				}
			};

			DrawGridSlot(puyo.x / Puyo::Width(), puyo.y / Puyo::Height());

			puyo.colour = Puyo::RandomColour();
			puyo.y = -puyo.Height();

			bool redraw = false;

			for (unsigned int y = 0; y < left_grid.size(); ++y)
			{
				const auto &row = left_grid[y];
				const auto &first_tile = row[0];

				if (first_tile == GridSlot::NOTHING)
					continue;

				bool same = true;

				for (const auto &tile : row)
				{
					if (tile != first_tile)
					{
						same = false;
						break;
					}
				}

				if (same)
				{
					for (unsigned int i = y; i > 0; --i)
						left_grid[i] = left_grid[i - 1];

					left_grid[0].fill(GridSlot::NOTHING);

					redraw = true;
				}
			}

			if (redraw)
				for (unsigned int y = 0; y < left_grid.size(); ++y)
					for (unsigned int x = 0; x < left_grid[0].size(); ++x)
						DrawGridSlot(x, y);
		}

		if (joypads[0].pressed.left && puyo.Left() != 0 && left_grid[puyo.Bottom() / puyo.Height()][puyo.x / puyo.Width() - 1] == GridSlot::NOTHING)
			puyo.x -= Puyo::Width();
		if (joypads[0].pressed.right && puyo.Right() != GRID_WIDTH && left_grid[puyo.Bottom() / puyo.Height()][puyo.x / puyo.Width() + 1] == GridSlot::NOTHING)
			puyo.x += Puyo::Width();
	}
}

[[noreturn]] static void ErrorTrap()
{
	MD::VDP::CRAM::Fill({7, 0, 0});

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

void _Level6InterruptHandler()
{
	if (!waiting_for_v_int)
		return;

	waiting_for_v_int = false;

	const MD::VDP::VRAM::Sprite sprite{.y = 0x80 + GRID_Y + puyo.y, .width = 1, .height = 1, .link = 0, .tile_metadata = {.priority = false, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = GridSlotToTileIndex(PuyoColourToGridSlot(puyo.colour))}, .x = 0x80 + LEFT_GRID_X + puyo.x};
	MD::VDP::CopyWordsWithoutDMA(MD::VDP::RAM::VRAM, VRAM_SPRITE_TABLE, &sprite, sizeof(sprite) / 2);

	joypad_manager.Update();
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
