#ifndef SPRITE_H
#define SPRITE_H

#include <clownmdsdk.h>

#include "objects/base.h"

using namespace ClownMDSDK::MainCPU;

namespace Sprite
{
	extern std::array<VDP::VRAM::Sprite, 80> table;
	extern unsigned char total_sprites;

	void Queue(unsigned int x, unsigned int y, unsigned int width, unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata);
	void UploadTable(Z80::Bus &z80_bus);
}

#endif // SPRITE_H
