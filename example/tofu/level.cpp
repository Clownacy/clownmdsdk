#include "level.h"

static constexpr unsigned int plane_width_in_tiles = 64;
static constexpr unsigned int plane_height_in_tiles = 32;

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
	const Coordinate::Tile camera_tile_position(camera_block_position); 
	auto vram_address = 0xC000 + sizeof(VDP::VRAM::TileMetadata) * (plane_width_in_tiles * camera_tile_position.y + camera_tile_position.x);

	for (unsigned int block_in_screen_y = 0; block_in_screen_y < screen_height / Coordinate::block_height_in_pixels + 1; ++block_in_screen_y)
	{
		constexpr unsigned int line_length_in_blocks = screen_width / Coordinate::block_width_in_pixels + 1;
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
			z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, vram_address, std::data(tile_metadata_lines[tile_in_block_y]), std::size(tile_metadata_lines[tile_in_block_y]));
			vram_address += sizeof(VDP::VRAM::TileMetadata) * plane_width_in_tiles;
		}
	}
}
