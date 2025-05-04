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

		template<std::size_t Index>
		auto& Dimension(this auto &&self)
		{
			if constexpr (Index == 0)
				return self.x;
			else //if constexpr (Index == 1)
				return self.y;
		}

		constexpr World ToWorld(this auto &&self);
		constexpr Pixel ToPixel(this auto &&self);
		constexpr Tile ToTile(this auto &&self);
		constexpr Block ToBlock(this auto &&self);

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

	static constexpr Tile block_size_in_tiles(2, 2);

	// TODO: Delete this.
	static constexpr unsigned int block_width_in_tiles = block_size_in_tiles.x;
	static constexpr unsigned int block_height_in_tiles = block_size_in_tiles.y;

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
		: Base(block.x * block_width_in_tiles, block.y * block_height_in_tiles)
	{}

	constexpr inline Block::Block(const Tile &tile)
		: Base(tile.x / block_width_in_tiles, tile.y / block_height_in_tiles)
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
	constexpr inline Pixel Base<Integer>::ToPixel(this auto &&self)
	{
		return self;
	}

	template<typename Integer>
	constexpr inline Tile Base<Integer>::ToTile(this auto &&self)
	{
		return self;
	}

	template<typename Integer>
	constexpr inline Block Base<Integer>::ToBlock(this auto &&self)
	{
		return self;
	}

	// TODO: Delete this.
	static constexpr unsigned int block_width_in_pixels = block_size_in_tiles.ToPixel().x;
	static constexpr unsigned int block_height_in_pixels = block_size_in_tiles.ToPixel().y;
}

#endif // COORDINATE_H
