#ifndef LEVEL_H
#define LEVEL_H

#include <array>

#include <clownmdsdk.h>

#include "coordinate.h"

using namespace ClownMDSDK::MainCPU;

class LevelManager
{
private:
	static const std::array<unsigned char, Coordinate::level_size.blocks.x * Coordinate::level_size.blocks.y> blocks;

	Coordinate::Block camera_previous;

	template<bool vertical>
	void DrawBlocks(Z80::Bus &z80_bus, const Coordinate::Block &starting_block_position, unsigned int total_lines);
	void DrawRows(Z80::Bus &z80_bus, const Coordinate::Block &starting_block_position, unsigned int total_rows);
	void DrawColumns(Z80::Bus &z80_bus, const Coordinate::Block &starting_block_position, unsigned int total_columns);

public:
	Coordinate::Pixel camera;

	void DrawWholeScreen(Z80::Bus &z80_bus);
	void Draw(Z80::Bus &z80_bus);

	[[nodiscard]] inline const auto& GetBlock(const Coordinate::Block &position)
	{
		return blocks[position.y * Coordinate::level_size.blocks.x + position.x];
	}
};

extern LevelManager level_manager;

#endif // LEVEL_H
