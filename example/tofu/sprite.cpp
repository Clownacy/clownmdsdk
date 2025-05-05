#include "sprite.h"

#include "level.h"

std::array<VDP::VRAM::Sprite, 80> Sprite::table;
unsigned char Sprite::total_sprites;

void Sprite::Queue(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const VDP::VRAM::TileMetadata &tile_metadata)
{
	if (total_sprites == std::size(table))
		return;

	static constexpr auto OffsetCoordinate = [](const unsigned long position, const unsigned int size)
	{
		return position - (size + 1) * 8 / 2;
	};

	const auto screen_x = OffsetCoordinate(x, width ) - Level::camera.x;
	const auto screen_y = OffsetCoordinate(y, height) - Level::camera.y;

	if (screen_x >= 0x200 || screen_y >= 0x200)
		return;

	auto &sprite = table[total_sprites++];
	sprite.x = screen_x;
	sprite.y = screen_y;
	sprite.width = width;
	sprite.height = height;
	sprite.link = total_sprites;
	sprite.tile_metadata = tile_metadata;	
}

void Sprite::UploadTable(Z80::Bus &z80_bus)
{
	if (total_sprites == 0)
	{
		// Blank the first sprite table entry, since the list cannot be 0 entries long.
		table[0] = {};
		total_sprites = 1;
	}
	else
	{
		// Terminate the sprite list.
		table[total_sprites - 1].link = 0;
	}

	z80_bus.CopyWordsToVDPWithDMA(VDP::RAM::VRAM, 0xD800, std::data(table), total_sprites * sizeof(VDP::VRAM::Sprite) / 2);
	total_sprites = 0;
}
