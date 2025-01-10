#include "main-menu.h"

#include <array>

#include "command.h"
#include "control-pad.h"
#include "mode.h"
#include "system.h"
#include "utility.h"

struct OptionsMenuEntry
{
	ModeID mode;
	const char *label;
};

static constexpr auto options = std::to_array<OptionsMenuEntry>({
	{ModeID::CDC_TEST     , "CDC Test"     },
	{ModeID::GRAPHICS_TEST, "Graphics Test"},
});

void MainMenu::DoEverything()
{
	MD::VDP::SetAddressIncrement(1);
	MD::VDP::VRAM::FillBytesWithDMA(VRAM_PLANE_A, PLANE_WIDTH * PLANE_HEIGHT * 2, 0);
	MD::VDP::WaitUntilDMAIsComplete();
	MD::VDP::SetAddressIncrement(2);

	const auto tile_y = SCREEN_HEIGHT / MD::VDP::VRAM::TILE_HEIGHT_NORMAL / 2;

	for (unsigned int i = 0; i < std::size(options); ++i)
	{
		SetupPlaneWrite(8, tile_y - option_index * 2 + i * 2);
		DrawString(options[i].label, i == option_index ? 0 : 1);
	}
}

MainMenu::MainMenu()
{
	DoEverything();
}

ModeID MainMenu::Update()
{
	// Cycle options.
	const auto old_option_index = option_index;

	if (control_pads[0].pressed.start || control_pads[0].pressed.a || control_pads[0].pressed.c)
		return options[option_index].mode;

	if (control_pads[0].pressed.up)
	{
		if (option_index == 0)
			option_index = std::size(options) - 1;
		else
			--option_index;
	}
	if (control_pads[0].pressed.down)
	{
		if (option_index == std::size(options) - 1)
			option_index = 0;
		else
			++option_index;
	}

	if (old_option_index != option_index)
		DoEverything();

	return ModeID::UNCHANGED;
}
