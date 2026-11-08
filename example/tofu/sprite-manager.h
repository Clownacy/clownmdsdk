#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "mega-drive.h"
#include "sprite-table.h"
#include "vram-addresses.h"

class SpriteManager
{
private:
	SpriteTable sprite_table;

public:
	void QueueSprite(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata);
	void UploadTable(Z80::Bus &z80_bus)
	{
		sprite_table.UploadTable(z80_bus, VRAMAddresses::SpriteTable);
	}
};

extern SpriteManager sprite_manager;

#endif // SPRITE_MANAGER_H
