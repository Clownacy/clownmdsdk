#ifndef VRAM_ADDRESSES_H
#define VRAM_ADDRESSES_H

namespace VRAMAddresses
{
	static constexpr auto PlaneA = 0xC000;
	static constexpr auto SpriteTable = 0xD800;
	static constexpr auto HorizontalScrollTable = 0xDC00;
	static constexpr auto WindowPlane = 0xF000;
}

#endif // VRAM_ADDRESSES_H
