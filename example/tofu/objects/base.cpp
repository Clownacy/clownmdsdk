#include "base.h"

#include "../sprite-manager.h"

void Objects::Base::QueueForDisplay(const int x_offset, const int y_offset, const unsigned int width, const unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata)
{
	const auto pixel_position = position.ToPixels();
	sprite_manager.QueueSprite(0x80 + pixel_position.x + x_offset, 0x80 + pixel_position.y + y_offset, width, height, tile_metadata);
}
