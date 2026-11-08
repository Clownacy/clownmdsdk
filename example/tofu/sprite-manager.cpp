#include "sprite-manager.h"

#include "level-manager.h"

SpriteManager sprite_manager;

void SpriteManager::QueueSprite(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata)
{
	if (sprite_table.Full())
		return;

	static constexpr auto OffsetCoordinate = [](const unsigned long position, const unsigned int size)
	{
		return position - (size + 1) * 8 / 2;
	};

	const auto screen_x = OffsetCoordinate(x, width ) - level_manager.camera.x;
	const auto screen_y = OffsetCoordinate(y, height) - level_manager.camera.y;

	// Cull sprites that are off-screen.
	const unsigned int maximum_sprite_radius = 16;
	const unsigned int left_boundary = 0x80;
	const unsigned int right_boundary = left_boundary + Coordinate::screen_size.pixels.x;
	const unsigned int top_boundary = 0x80;
	const unsigned int bottom_boundary = top_boundary + Coordinate::screen_size.pixels.y;

	if (screen_x <= left_boundary   - maximum_sprite_radius
	 || screen_x >= right_boundary  + maximum_sprite_radius
	 || screen_y <= top_boundary    - maximum_sprite_radius
	 || screen_y >= bottom_boundary + maximum_sprite_radius)
		return;

	sprite_table.QueueSprite(screen_x, screen_y, width, height, tile_metadata);
}
