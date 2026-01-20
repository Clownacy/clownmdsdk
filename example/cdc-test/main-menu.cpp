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
	{ModeID::HV_TEST_H_INT        , "H/V at H-Int"},
	{ModeID::HV_TEST_V_START      , "H/V at V-Blank Start"},
	{ModeID::HV_TEST_V_END        , "H/V at V-Blank End"},
	{ModeID::HV_TEST_H_START      , "H/V at H-Blank Start"},
	{ModeID::HV_TEST_H_END        , "H/V at H-Blank End"},
});

void MainMenu::DoEverything()
{
	ClearPlaneA();

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
