#include "level.h"

#include <numeric>
#include <utility>

namespace Level
{

const std::array<unsigned char, Coordinate::level_size.blocks.x * Coordinate::level_size.blocks.y> blocks = {
	#embed "assets/level.unc"
};

Coordinate::Pixel camera;
static Coordinate::Block camera_previous;

static constexpr auto SplitLength(const auto offset, const auto length, const auto limit)
{
	const auto second_length = std::sub_sat(offset + length, limit);
	const auto first_length = length - second_length;

	return std::pair(first_length, second_length);
}

template<bool vertical>
static void DrawBlocks(Z80::Bus &z80_bus, const Coordinate::Block &starting_block_position, const unsigned int total_lines)
{
	constexpr auto line_stride = sizeof(VDP::VRAM::TileMetadata) * (vertical ? 1 : Coordinate::plane_size.tiles.x);
	const auto starting_tile_position_in_plane = starting_block_position.ToTiles() % Coordinate::plane_size.tiles;

	// We split the transfer in two to handle wrapping around the plane.
	constexpr unsigned int line_length_in_blocks = Coordinate::screen_size.pixels.Dimension(vertical) / Coordinate::block_size.pixels.Dimension(vertical) + 1;
	constexpr unsigned int line_length_in_tiles = line_length_in_blocks * Coordinate::block_size.tiles.Dimension(vertical);
	const auto [first_transfer_length, second_transfer_length] = SplitLength(starting_tile_position_in_plane.Dimension(vertical), line_length_in_tiles, Coordinate::plane_size.tiles.Dimension(vertical));

	auto block_position = starting_block_position;
	VDP::VRAM::TileMetadata tile_metadata{.priority = true, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 0};

	const auto DoLine = [&](const unsigned int vram_offset_x, const unsigned int vram_offset_y, const unsigned int total_lines)
	{
		auto second_transfer_vram_address = 0xC000 + (vertical ? vram_offset_x : vram_offset_y);
		auto first_transfer_vram_address = second_transfer_vram_address + (vertical ? vram_offset_y : vram_offset_x);

		for (unsigned int block_line_in_screen = 0; block_line_in_screen < total_lines; ++block_line_in_screen)
		{
			std::array<std::array<VDP::VRAM::TileMetadata, line_length_in_blocks * Coordinate::block_size.tiles.Dimension(vertical)>, Coordinate::block_size.tiles.Dimension(!vertical)> tile_metadata_lines;

			auto *block_pointer = &GetBlock(block_position);

			++block_position.Dimension(!vertical);

			unsigned int tile_in_line = 0;

			for (unsigned int block_in_screen = 0; block_in_screen < line_length_in_blocks; ++block_in_screen)
			{
				tile_metadata.tile_index = *block_pointer * Coordinate::block_size.tiles.x * Coordinate::block_size.tiles.y;
				block_pointer += vertical ? Coordinate::level_size.blocks.x : 1;

				for (unsigned int tile_in_block_x = 0; tile_in_block_x < Coordinate::block_size.tiles.x; ++tile_in_block_x)
				{
					for (unsigned int tile_in_block_y = 0; tile_in_block_y < Coordinate::block_size.tiles.y; ++tile_in_block_y)
					{
						tile_metadata_lines[vertical ? tile_in_block_x : tile_in_block_y][tile_in_line + (vertical ? tile_in_block_y : tile_in_block_x)] = tile_metadata;
						++tile_metadata.tile_index;
					}
				}

				tile_in_line += Coordinate::block_size.tiles.Dimension(vertical);
			}

			for (const auto &tile_metadata_line : tile_metadata_lines)
			{
				z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, first_transfer_vram_address, &tile_metadata_line[0], first_transfer_length);
				first_transfer_vram_address += line_stride;

				if (second_transfer_length != 0)
				{
					z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, second_transfer_vram_address, &tile_metadata_line[first_transfer_length], second_transfer_length);
					second_transfer_vram_address += line_stride;
				}
			}
		}
	};

	// We split the lines we draw into two batches as well, also to handle plane wrapping.
	const auto [first_line_length, second_line_length] = SplitLength(starting_tile_position_in_plane.ToBlocks().Dimension(!vertical), total_lines, Coordinate::plane_size.tiles.ToBlocks().Dimension(!vertical));

	const auto vram_offset_x = sizeof(VDP::VRAM::TileMetadata) * starting_tile_position_in_plane.x;
	const auto vram_offset_y = sizeof(VDP::VRAM::TileMetadata) * starting_tile_position_in_plane.y * Coordinate::plane_size.tiles.x;

	DoLine(vram_offset_x, vram_offset_y, first_line_length);
	DoLine(vertical ? 0 : vram_offset_x, vertical ? vram_offset_y : 0, second_line_length);
}

static void DrawRows(Z80::Bus &z80_bus, const Coordinate::Block &starting_block_position, const unsigned int total_rows)
{
	DrawBlocks<false>(z80_bus, starting_block_position, total_rows);
}

static void DrawColumns(Z80::Bus &z80_bus, const Coordinate::Block &starting_block_position, const unsigned int total_columns)
{
	// Ensure that the DMA transfers write along the Y axis rather than the X axis.
	VDP::SetAddressIncrement(sizeof(VDP::VRAM::TileMetadata) * Coordinate::plane_size.tiles.x);

	DrawBlocks<true>(z80_bus, starting_block_position, total_columns);

	// Restore the default increment.
	VDP::SetAddressIncrement(2);
}

void DrawWholeScreen(Z80::Bus &z80_bus)
{
	// Chooses whichever is more efficient for the screen resolution.
	if constexpr (Coordinate::screen_size.blocks.x >= Coordinate::screen_size.blocks.y)
		DrawRows(z80_bus, camera, Coordinate::screen_size.pixels.y / Coordinate::block_size.pixels.y + 1);
	else
		DrawColumns(z80_bus, camera, Coordinate::screen_size.pixels.x / Coordinate::block_size.pixels.x + 1);

	camera_previous = camera;
}

// Checks how much the camera has moved in this frame, and loads blocks at the edges of the screen.
void Draw(Z80::Bus &z80_bus)
{
	const auto camera_block_position = camera.ToBlocks();

	// TODO: This assumes that the camera cannot move more than one block per frame.
	if (camera_block_position.x < camera_previous.x)
		DrawColumns(z80_bus, camera_block_position, 1);
	else if (camera_block_position.x > camera_previous.x)
		DrawColumns(z80_bus, camera_block_position + Coordinate::Block(Coordinate::screen_size.blocks.x, 0), 1);

	if (camera_block_position.y < camera_previous.y)
		DrawRows(z80_bus, camera_block_position, 1);
	else if (camera_block_position.y > camera_previous.y)
		DrawRows(z80_bus, camera_block_position + Coordinate::Block(0, Coordinate::screen_size.blocks.y), 1);

	camera_previous = camera_block_position;
}

}
