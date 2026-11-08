#include "sprite-table.h"

#include <assert.h>

void SpriteTable::QueueSprite(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata)
{
	_assertm(total_sprites < std::size(table), "Tried to push sprite to a full sprite table.");

	auto &sprite = table[total_sprites++];
	sprite.x = x;
	sprite.y = y;
	sprite.width = width;
	sprite.height = height;
	sprite.link = total_sprites;
	sprite.tile_metadata = tile_metadata;	
}

void SpriteTable::UploadTable(Z80::Bus &z80_bus, const unsigned int vram_address)
{
	if (total_sprites == 0)
	{
		// Write a dummy first sprite, since the list cannot be 0 entries long.
		VDP::SendCommand(VDP::RAM::VRAM, VDP::Access::WRITE, vram_address);
		VDP::Write(VDP::DataValueLongword(0));
	}
	else
	{
		// Terminate the sprite list and upload it
		table[total_sprites - 1].link = 0;
		z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, vram_address, std::data(table), total_sprites * sizeof(VDP::VRAM::Sprite) / sizeof(short));
		total_sprites = 0;
	}
}
