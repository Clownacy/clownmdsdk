#ifndef SPRITE_H
#define SPRITE_H

#include <clownmdsdk.h>

#include "objects/base.h"

using namespace ClownMDSDK::MainCPU;

class SpriteManager
{
private:
	std::array<VDP::VRAM::Sprite, 80> table;
	unsigned char total_sprites;

public:
	void QueueSprite(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata);
	void UploadTable(Z80::Bus &z80_bus);
};

extern SpriteManager sprite_manager;

#endif // SPRITE_H
