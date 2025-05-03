#include "base.h"

#include "../sprite.h"

void Objects::Base::QueueForDisplay(const int x_offset, const int y_offset, const unsigned int width, const unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata)
{
	static constexpr auto WorldCoordinateToScreen = [](const unsigned long coordinate) -> unsigned int
	{
		return 0x80 + coordinate / 0x10000;
	};

	Sprite::Queue(WorldCoordinateToScreen(position.x) + x_offset, WorldCoordinateToScreen(position.y) + y_offset, width, height, tile_metadata);
}
