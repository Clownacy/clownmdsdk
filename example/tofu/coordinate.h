#ifndef COORDINATE_H
#define COORDINATE_H

#include <type_traits>

#include <clownmdsdk.h>

#define COORDINATE_BASE_MAKE_OPERATOR(OPERATOR) \
	template<typename Self> \
	Self operator OPERATOR(this const Self &self, const Self &other) \
	{ \
		return {self.x OPERATOR other.x, self.y OPERATOR other.y}; \
	} \
	template<typename Self> \
	Self operator OPERATOR(this const Self &self, const Integer value) \
	{ \
		return {self.x OPERATOR value, self.y OPERATOR value}; \
	}

using namespace ClownMDSDK::MainCPU;

namespace Coordinate
{
	class World;
	class Pixel;
	class Tile;
	class Block;

	template<typename Integer>
	class Base
	{
	public:
		Integer x, y;

		constexpr Base() {};
		constexpr Base(const Integer x, const Integer y)
			: x(x), y(y)
		{}

		constexpr auto& Dimension(this auto &&self, const std::size_t index)
		{
			if (index == 0)
				return self.x;
			else //if (index == 1)
				return self.y;

			_assertm(index < 2, "Out-of-bounds coordinate index.");
		}

		constexpr World ToWorld(this auto &&self);
		constexpr Pixel ToPixels(this auto &&self);
		constexpr Tile ToTiles(this auto &&self);
		constexpr Block ToBlocks(this auto &&self);

		COORDINATE_BASE_MAKE_OPERATOR(+)
		COORDINATE_BASE_MAKE_OPERATOR(-)
		COORDINATE_BASE_MAKE_OPERATOR(*)
		COORDINATE_BASE_MAKE_OPERATOR(/)
		COORDINATE_BASE_MAKE_OPERATOR(%)
	};

	class World : public Base<unsigned long>
	{
	public:
		using Base::Base;

		constexpr World(const Pixel &pixel);
		constexpr World(const Tile &tile);
		constexpr World(const Block &block);
	};

	class Pixel : public Base<unsigned int>
	{
	public:
		using Base::Base;

		constexpr Pixel(const World &world);
		constexpr Pixel(const Tile &tile);
		constexpr Pixel(const Block &block);
	};

	class Tile : public Base<unsigned int>
	{
	public:
		using Base::Base;

		constexpr Tile(const World &world);
		constexpr Tile(const Pixel &pixel);
		constexpr Tile(const Block &block);
	};

	class Block : public Base<unsigned int>
	{
	public:
		using Base::Base;

		constexpr Block(const Tile &tile);
		constexpr Block(const Pixel &pixel);
		constexpr Block(const World &world);
	};

	constexpr inline World::World(const Pixel &pixel)
		: Base(pixel.x * 0x10000, pixel.y * 0x10000)
	{}

	constexpr inline World::World(const Tile &tile)
		: World(Pixel(tile))
	{}

	constexpr inline World::World(const Block &block)
		: World(Tile(block))
	{}

	constexpr inline Pixel::Pixel(const World &world)
		: Base(world.x / 0x10000, world.y / 0x10000)
	{}

	constexpr inline Pixel::Pixel(const Tile &tile)
		: Base(tile.x * VDP::VRAM::TILE_WIDTH, tile.y * VDP::VRAM::TILE_HEIGHT_NORMAL)
	{}

	constexpr inline Pixel::Pixel(const Block &block)
		: Pixel(Tile(block))
	{}

	constexpr inline Tile::Tile(const World &world)
		: Tile(Pixel(world))
	{}

	constexpr inline Tile::Tile(const Pixel &pixel)
		: Base(pixel.x / VDP::VRAM::TILE_WIDTH, pixel.y / VDP::VRAM::TILE_HEIGHT_NORMAL)
	{}

	constexpr inline Tile::Tile(const Block &block)
		: Base(block.x * 2, block.y * 2)
	{}

	constexpr inline Block::Block(const Tile &tile)
		: Base(tile.x / 2, tile.y / 2)
	{}

	constexpr inline Block::Block(const Pixel &pixel)
		: Block(Tile(pixel))
	{}

	constexpr inline Block::Block(const World &world)
		: Block(Pixel(world))
	{}

	template<typename Integer>
	constexpr inline World Base<Integer>::ToWorld(this auto &&self)
	{
		return self;
	}

	template<typename Integer>
	constexpr inline Pixel Base<Integer>::ToPixels(this auto &&self)
	{
		return self;
	}

	template<typename Integer>
	constexpr inline Tile Base<Integer>::ToTiles(this auto &&self)
	{
		return self;
	}

	template<typename Integer>
	constexpr inline Block Base<Integer>::ToBlocks(this auto &&self)
	{
		return self;
	}

	class Omni
	{
	public:
		const World world;
		const Pixel pixels;
		const Tile tiles;
		const Block blocks;

		constexpr Omni(const auto &coordinate)
			: world(coordinate)
			, pixels(coordinate)
			, tiles(coordinate)
			, blocks(coordinate)
		{}
	};

	static constexpr Omni block_size(Block(1, 1));
	static constexpr Omni screen_size(Pixel(320, 224));
	static constexpr Omni level_size(Block(0x40, 0x10));
}

#endif // COORDINATE_H
