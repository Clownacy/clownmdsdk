#ifndef LEVEL_H
#define LEVEL_H

#include <array>

#include <clownmdsdk.h>

#include "coordinate.h"

using namespace ClownMDSDK::MainCPU;

namespace Level
{
	extern const std::array<unsigned char, Coordinate::level_size.blocks.x * Coordinate::level_size.blocks.y> blocks;
	extern Coordinate::Pixel camera;

	void DrawWholeScreen(Z80::Bus &z80_bus);
	void Draw(Z80::Bus &z80_bus);

	[[nodiscard]] inline const auto& GetBlock(const Coordinate::Block &position)
	{
		return blocks[position.y * Coordinate::level_size.blocks.x + position.x];
	}
}

#endif // LEVEL_H
