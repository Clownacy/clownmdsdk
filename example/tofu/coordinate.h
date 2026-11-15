#ifndef COORDINATE_H
#define COORDINATE_H

#include <cassert>
#include <concepts>
#include <utility>

#include "mega-drive.h"

#define COORDINATE_BASE_MAKE_OPERATOR(OPERATOR) \
	constexpr auto operator OPERATOR(this auto &&self, std::integral auto &&other) -> std::remove_cvref_t<decltype(self)> \
	{ \
		return {self.x OPERATOR other, self.y OPERATOR other}; \
	} \
	constexpr auto operator OPERATOR(this auto &&self, const std::remove_cvref_t<decltype(self)> &other) -> std::remove_cvref_t<decltype(self)> \
	{ \
		return {self.x OPERATOR other.x, self.y OPERATOR other.y}; \
	} \
	constexpr auto& operator OPERATOR##=(auto &&other) \
	{ \
		return *this = *this OPERATOR std::forward<decltype(other)>(other); \
	}

namespace Coordinate
{
	class World;
	class Pixel;
	class Tile;
	class Block;

	template<std::integral Integer>
	class Base
	{
	public:
		Integer x, y;

		constexpr Base() {};
		constexpr Base(const Integer x, const Integer y)
			: x(x), y(y)
		{}

		template<std::size_t index>
		constexpr auto& Dimension(this auto &&self)
		{
			static_assert(index < 2, "Out-of-bounds coordinate dimension index.");

			switch (index)
			{
				default:
					std::unreachable();

				case 0:
					return self.x;

				case 1:
					return self.y;
			}
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

		constexpr explicit World(const Pixel &pixel);
		constexpr explicit World(const Tile &tile);
		constexpr explicit World(const Block &block);
	};

	class Pixel : public Base<unsigned int>
	{
	public:
		using Base::Base;

		constexpr explicit Pixel(const World &world);
		constexpr explicit Pixel(const Tile &tile);
		constexpr explicit Pixel(const Block &block);
	};

	class Tile : public Base<unsigned int>
	{
	public:
		using Base::Base;

		constexpr explicit Tile(const World &world);
		constexpr explicit Tile(const Pixel &pixel);
		constexpr explicit Tile(const Block &block);
	};

	class Block : public Base<unsigned int>
	{
	public:
		using Base::Base;

		constexpr explicit Block(const Tile &tile);
		constexpr explicit Block(const Pixel &pixel);
		constexpr explicit Block(const World &world);
	};

	constexpr inline World::World(const Pixel &pixel)
		: Base(pixel.x * 0x10000UL, pixel.y * 0x10000UL)
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

	template<std::integral Integer>
	constexpr inline World Base<Integer>::ToWorld(this auto &&self)
	{
		return World(self);
	}

	template<std::integral Integer>
	constexpr inline Pixel Base<Integer>::ToPixels(this auto &&self)
	{
		return Pixel(self);
	}

	template<std::integral Integer>
	constexpr inline Tile Base<Integer>::ToTiles(this auto &&self)
	{
		return Tile(self);
	}

	template<std::integral Integer>
	constexpr inline Block Base<Integer>::ToBlocks(this auto &&self)
	{
		return Block(self);
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
	static constexpr Omni plane_size(Tile(64, 32));
	static constexpr Omni hud_size(Tile(12, 3));
}

#endif // COORDINATE_H
