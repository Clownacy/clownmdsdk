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

	if (screen_x >= 0x200 || screen_y >= 0x200)
		return;

	sprite_table.QueueSprite(screen_x, screen_y, width, height, tile_metadata);
}
