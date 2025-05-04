#include "level.h"

#include <numeric>

namespace Level
{

static constexpr Coordinate::Tile plane_size_in_tiles(64, 32);

static constexpr unsigned int screen_width = screen_size.x;
static constexpr unsigned int screen_height = screen_size.y;

static constexpr unsigned int line_length_in_blocks = screen_width / Coordinate::block_width_in_pixels + 1;

const std::array<unsigned char, level_width_in_blocks * level_height_in_blocks> blocks = {
	#embed "assets/level.unc"
};

Coordinate::Pixel camera;
static Coordinate::Block camera_previous;

static void DrawRow(
	Z80::Bus &z80_bus,
	const Coordinate::Block &block_row_position,
	unsigned int first_transfer_vram_address,
	unsigned int second_transfer_vram_address,
	const unsigned int first_transfer_length,
	const unsigned int second_transfer_length
)
{
	std::array<std::array<VDP::VRAM::TileMetadata, line_length_in_blocks * Coordinate::block_width_in_tiles>, Coordinate::block_height_in_tiles> tile_metadata_lines;
	VDP::VRAM::TileMetadata tile_metadata{.priority = false, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 0};

	auto *tile = &GetBlock(block_row_position);

	unsigned int tile_in_screen_x = 0;

	for (unsigned int block_in_screen_x = 0; block_in_screen_x < line_length_in_blocks; ++block_in_screen_x)
	{
		tile_metadata.tile_index = *tile++ * Coordinate::block_width_in_tiles * Coordinate::block_height_in_tiles;

		for (unsigned int tile_in_block_x = 0; tile_in_block_x < Coordinate::block_width_in_tiles; ++tile_in_block_x)
		{
			for (unsigned int tile_in_block_y = 0; tile_in_block_y < Coordinate::block_height_in_tiles; ++tile_in_block_y)
			{
				tile_metadata_lines[tile_in_block_y][tile_in_screen_x] = tile_metadata;
				++tile_metadata.tile_index;
			}

			++tile_in_screen_x;
		}
	}

	for (unsigned int tile_in_block_y = 0; tile_in_block_y < Coordinate::block_height_in_tiles; ++tile_in_block_y)
	{
		z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, first_transfer_vram_address, &tile_metadata_lines[tile_in_block_y][0], first_transfer_length );
		first_transfer_vram_address += sizeof(VDP::VRAM::TileMetadata) * plane_size_in_tiles.x;

		if (second_transfer_length != 0)
		{
			z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, second_transfer_vram_address, &tile_metadata_lines[tile_in_block_y][first_transfer_length], second_transfer_length);
			second_transfer_vram_address += sizeof(VDP::VRAM::TileMetadata) * plane_size_in_tiles.x;
		}
	}
}

static void DrawRows(Z80::Bus &z80_bus, const Coordinate::Block &starting_block_position, const unsigned int total_rows)
{
	const auto plane_block_row_size_in_word = sizeof(VDP::VRAM::TileMetadata) * plane_size_in_tiles.x * Coordinate::block_height_in_tiles;

	const auto starting_tile_position_in_plane = starting_block_position.ToTile() % plane_size_in_tiles;

	// We split the transfer in two to handle wrapping around the plane.
	constexpr unsigned int line_length_in_tiles = line_length_in_blocks * Coordinate::block_width_in_tiles;
	const auto second_transfer_length = std::sub_sat(starting_tile_position_in_plane.x + line_length_in_tiles, plane_size_in_tiles.x);
	const auto first_transfer_length = line_length_in_tiles - second_transfer_length;

	auto second_transfer_vram_address = 0xC000 + sizeof(VDP::VRAM::TileMetadata) * plane_size_in_tiles.x * starting_tile_position_in_plane.y;
	auto first_transfer_vram_address = second_transfer_vram_address + sizeof(VDP::VRAM::TileMetadata) * starting_tile_position_in_plane.x;

	for (unsigned int block_in_screen_y = 0; block_in_screen_y < total_rows; ++block_in_screen_y)
	{
		DrawRow(z80_bus, {starting_block_position + Coordinate::Block(0, block_in_screen_y)}, first_transfer_vram_address, second_transfer_vram_address, first_transfer_length, second_transfer_length);
		first_transfer_vram_address += plane_block_row_size_in_word;
		second_transfer_vram_address += plane_block_row_size_in_word;
	}
}

void DrawWholeScreen(Z80::Bus &z80_bus)
{
	DrawRows(z80_bus, camera, screen_height / Coordinate::block_height_in_pixels + 1);

	camera_previous = camera;
}

void Draw(Z80::Bus &z80_bus)
{
//	DrawWholeScreen(z80_bus);
//	return;
	const auto camera_block_position = camera.ToBlock();

	if (camera_block_position.y < camera_previous.y)
		DrawRows(z80_bus, camera_block_position, 1);
	else if (camera_block_position.y > camera_previous.y)
		DrawRows(z80_bus, camera_block_position + Coordinate::Block(0, screen_size.ToBlock().y), 1);

	camera_previous = camera_block_position;
}

}
