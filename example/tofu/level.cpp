#include "level.h"

#include <numeric>

static constexpr Coordinate::Tile plane_size_in_tiles(64, 32);

static constexpr unsigned int screen_width = Level::screen_size.x;
static constexpr unsigned int screen_height = Level::screen_size.y;

const std::array<unsigned char, Level::level_width_in_blocks * Level::level_height_in_blocks> Level::blocks = {
	#embed "assets/level.unc"
};

Coordinate::Pixel Level::camera;

void Level::Redraw(Z80::Bus &z80_bus)
{
	VDP::VRAM::TileMetadata tile_metadata{.priority = false, .palette_line = 0, .y_flip = false, .x_flip = false, .tile_index = 0};
	const Coordinate::Block camera_block_position(camera); 
	const auto camera_tile_position_in_plane = camera_block_position.ToTile() % plane_size_in_tiles;

	constexpr unsigned int line_length_in_blocks = screen_width / Coordinate::block_width_in_pixels + 1;
	constexpr unsigned int line_length_in_tiles = line_length_in_blocks * Coordinate::block_width_in_tiles;

	// We split the transfer in two to handle wrapping around the plane.
	const auto second_transfer_length = std::sub_sat(camera_tile_position_in_plane.x + line_length_in_tiles, plane_size_in_tiles.x);
	const auto first_transfer_length = line_length_in_tiles - second_transfer_length;

	auto second_transfer_vram_address = 0xC000 + sizeof(VDP::VRAM::TileMetadata) * plane_size_in_tiles.x * camera_tile_position_in_plane.y;
	auto first_transfer_vram_address = second_transfer_vram_address + sizeof(VDP::VRAM::TileMetadata) * camera_tile_position_in_plane.x;

	for (unsigned int block_in_screen_y = 0; block_in_screen_y < screen_height / Coordinate::block_height_in_pixels + 1; ++block_in_screen_y)
	{
		std::array<std::array<VDP::VRAM::TileMetadata, line_length_in_blocks * Coordinate::block_width_in_tiles>, Coordinate::block_height_in_tiles> tile_metadata_lines;

		auto tile = &GetBlock(camera_block_position + Coordinate::Block(0, block_in_screen_y));

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
}
