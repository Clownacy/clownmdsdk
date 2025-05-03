#ifndef LEVEL_H
#define LEVEL_H

#include <array>

#include <clownmdsdk.h>

#include "coordinate.h"

using namespace ClownMDSDK::MainCPU;

namespace Level
{
	static constexpr unsigned int level_width_in_blocks = 0x20;
	static constexpr unsigned int level_height_in_blocks = 0x10;
	extern const std::array<unsigned char, level_width_in_blocks * level_height_in_blocks> blocks;

	void Redraw(Z80::Bus &z80_bus);

	[[nodiscard]] inline const auto& GetBlock(const Coordinate::Block &position)
	{
		return blocks[position.y * level_width_in_blocks + position.x];
	}
}

#endif // LEVEL_H
