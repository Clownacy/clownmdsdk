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

void ClearPlaneA()
{
	MD::VDP::SetAddressIncrement(1);
	MD::VDP::VRAM::FillBytesWithDMA(VRAM_PLANE_A, PLANE_WIDTH * PLANE_HEIGHT * 2, 0);
	MD::VDP::WaitUntilDMAIsComplete();
	MD::VDP::SetAddressIncrement(2);
}
