#ifndef COORDINATE_H
#define COORDINATE_H

#include <type_traits>

#include <clownmdsdk.h>

#define COORDINATE_BASE_MAKE_OPERATOR(OPERATOR) \
	template<typename Self> \
	Self operator OPERATOR(this const Self &self, const Self &other) \
	{ \
		return {self.x OPERATOR other.x, self.y OPERATOR other.y}; \
	}

using namespace ClownMDSDK::MainCPU;

namespace Coordinate
{
	static constexpr unsigned int block_width_in_tiles = 2;
	static constexpr unsigned int block_width_in_pixels = block_width_in_tiles * VDP::VRAM::TILE_WIDTH;

	static constexpr unsigned int block_height_in_tiles = 2;
	static constexpr unsigned int block_height_in_pixels = block_height_in_tiles * VDP::VRAM::TILE_HEIGHT_NORMAL;

	class World;
	class Block;

	template<typename Integer>
	class Base
	{
	public:
		Integer x, y;

		Base() {};
		Base(const Integer x, const Integer y)
			: x(x), y(y)
		{}

		template<std::size_t Index>
		auto& Dimension(this auto &&self)
		{
			if constexpr (Index == 0)
				return self.x;
			else //if constexpr (Index == 1)
				return self.y;
		}

		COORDINATE_BASE_MAKE_OPERATOR(+)
		COORDINATE_BASE_MAKE_OPERATOR(-)
		COORDINATE_BASE_MAKE_OPERATOR(*)
		COORDINATE_BASE_MAKE_OPERATOR(/)
	};

	class World : public Base<unsigned long>
	{
	public:
		using Base::Base;

		World(const Block &block);
	};

	class Block : public Base<unsigned int>
	{
	public:
		using Base::Base;

		Block(const World &world);
	};

	inline World::World(const Block &block)
		: Base(block.x * 0x10000 * block_width_in_pixels, block.y * 0x10000 * block_height_in_pixels)
	{}

	inline Block::Block(const World &world)
		: Base(world.x / 0x10000 / block_width_in_pixels, world.y / 0x10000 / block_height_in_pixels)
	{}
}

#endif // COORDINATE_H
