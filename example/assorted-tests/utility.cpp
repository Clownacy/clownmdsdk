#include "utility.h"

#include "system.h"

void SetupPlaneWrite(const unsigned int x, unsigned int y)
{
	MD::VDP::SendCommand(MD::VDP::RAM::VRAM, MD::VDP::Access::WRITE, VRAM_PLANE_A + (y * PLANE_WIDTH + x) * 2);
}

void DrawString(const char* const string, const unsigned int palette_line)
{
	for (const char* character = string; *character != '\0'; ++character)
		MD::VDP::Write(MD::VDP::VRAM::TileMetadata{false, palette_line, false, false, static_cast<unsigned int>(*character)});
}

void DrawHexWord(const unsigned short value, const unsigned int palette_line)
{
	static constexpr unsigned int BITS_PER_NYBBLE = 4;
	static constexpr unsigned int NYBBLES_PER_VALUE = 16 / BITS_PER_NYBBLE;
	static constexpr unsigned int NYBBLE_MASK = (1 << BITS_PER_NYBBLE) - 1;

	for (unsigned int i = 0; i < NYBBLES_PER_VALUE; ++i)
	{
		static constexpr auto NybbleToASCII = [](const unsigned int nybble)
		{
			if (nybble >= 0xA)
				return nybble - 0xA + 'A';
			else
				return nybble - 0x0 + '0';
		};

		MD::VDP::Write(MD::VDP::VRAM::TileMetadata{false, palette_line, false, false, NybbleToASCII(value << i * BITS_PER_NYBBLE >> ((NYBBLES_PER_VALUE - 1) * BITS_PER_NYBBLE) & NYBBLE_MASK)});
	}
}

void ClearPlaneA()
{
	MD::VDP::SetAddressIncrement(1);
	MD::VDP::VRAM::FillBytesWithDMA(VRAM_PLANE_A, PLANE_WIDTH * PLANE_HEIGHT * 2, 0);
	MD::VDP::WaitUntilDMAIsComplete();
	MD::VDP::SetAddressIncrement(2);
}
