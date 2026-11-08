#ifndef SPRITE_TABLE_H
#define SPRITE_TABLE_H

#include "mega-drive.h"

class SpriteTable
{
private:
	std::array<VDP::VRAM::Sprite, 80> table;
	unsigned char total_sprites = 0;

public:
	void QueueSprite(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata);
	void UploadTable(Z80::Bus &z80_bus, unsigned int vram_address);
	bool Full() const
	{
		return total_sprites == std::size(table);
	}
};

#endif // SPRITE_TABLE_H
