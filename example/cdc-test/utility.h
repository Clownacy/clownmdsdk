#ifndef UTILITY_H
#define UTILITY_H

static constexpr unsigned int VRAM_PLANE_A = 0xC000;
static constexpr unsigned int PLANE_WIDTH = 64;
static constexpr unsigned int PLANE_HEIGHT = 32;
static constexpr unsigned int SCREEN_HEIGHT = 224;

void SetupPlaneWrite(const unsigned int x, unsigned int y);
void DrawString(const char* const string, const unsigned int palette_line = 0);
void DrawHexWord(const unsigned short value, const unsigned int palette_line = 0);
void ClearPlaneA();

#endif // UTILITY_H
