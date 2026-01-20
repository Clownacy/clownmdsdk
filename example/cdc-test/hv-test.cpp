#include "hv-test.h"

#include "command.h"
#include "control-pad.h"
#include "system.h"
#include "utility.h"

HVTest::HVTest(const unsigned int state)
	: state(state)
{
	ClearPlaneA();
	MD::VDP::SetHorizontalInterruptInterval(0);
}

HVTest::~HVTest()
{
	MD::VDP::Write(MD::VDP::Register00{.blank_leftmode_8_pixels = false, .enable_horizontal_interrupt = false, .lock_hv_counter = false});
}

static unsigned char ReadVCounter()
{
	unsigned char value;

	do
	{
		value = MD::VDP::v_counter;
	} while (value != MD::VDP::v_counter);

	return value;
}

ModeID HVTest::Update()
{
	switch (state)
	{
		case 0:
			ClearPlaneA();
			SetupPlaneWrite(2, 1);
			DrawString("Press A to sample V-counter.");
			++state;
			break;

		case 1:
			if (control_pads[0].pressed.a)
				++state;

			break;

		case 2:
			do_sample = true;
			++state;
			[[fallthrough]];
		case 3:
			if (!do_sample)
				state = 10;

			break;

		case 10:
			ClearPlaneA();

			for (unsigned int line = 0; line < 224 / 8; ++line)
			{
				SetupPlaneWrite(0, line);
				DrawHexWord(values[line].v_counter);
				SetupPlaneWrite(5, line);
				DrawHexWord(values[line].h_blank);
			}

			++state;

			break;

		case 14:
			if (control_pads[0].pressed.a)
				state = 0;

			break;

		case 20:
			line = 0;

			MD::M68k::DisableInterrupts();

			// Wait for V-blank to end.
			while (MD::VDP::ReadStatus().vertical_blanking);

			// Wait for V-blank to start.
			while (!MD::VDP::ReadStatus().vertical_blanking);

			values[line++] = {ReadVCounter(), MD::VDP::ReadStatus().horizontal_blanking};

			MD::M68k::SetInterruptMask(0);

			// Print.
			state = 10;

			break;

		case 30:
			line = 0;

			MD::M68k::DisableInterrupts();

			// Wait for V-blank to start.
			while (!MD::VDP::ReadStatus().vertical_blanking);

			// Wait for V-blank to end.
			while (MD::VDP::ReadStatus().vertical_blanking);

			values[line++] = {ReadVCounter(), MD::VDP::ReadStatus().horizontal_blanking};

			MD::M68k::SetInterruptMask(0);

			// Print.
			state = 10;

			break;

		case 40:
			line = 0;

			MD::M68k::DisableInterrupts();

			// Wait for V-blank to start.
			while (!MD::VDP::ReadStatus().vertical_blanking);

			// Wait for V-blank to end.
			while (MD::VDP::ReadStatus().vertical_blanking);

			for (unsigned int i = 0; i < 224; ++i)
			{
				// Wait for H-blank to start.
				while (!MD::VDP::ReadStatus().horizontal_blanking);

				// Log V-counter.
				values[line++] = {ReadVCounter(), MD::VDP::ReadStatus().horizontal_blanking};
			}

			MD::M68k::SetInterruptMask(0);

			// Print.
			state = 10;

			break;

		case 50:
			line = 0;

			MD::M68k::DisableInterrupts();

			// Wait for V-blank to start.
			while (!MD::VDP::ReadStatus().vertical_blanking);

			// Wait for V-blank to end.
			while (MD::VDP::ReadStatus().vertical_blanking);

			for (unsigned int i = 0; i < 224; ++i)
			{
				// Wait for H-blank to start.
				while (!MD::VDP::ReadStatus().horizontal_blanking);

				// Wait for H-blank to end.
				while (MD::VDP::ReadStatus().horizontal_blanking);

				// Log V-counter.
				values[line++] = {ReadVCounter(), MD::VDP::ReadStatus().horizontal_blanking};
			}

			MD::M68k::SetInterruptMask(0);

			// Print.
			state = 10;

			break;
	}

	if (control_pads[0].pressed.b)
		return ModeID::MAIN_MENU;

	return ModeID::UNCHANGED;
}

void HVTest::HorizontalInterrupt()
{
	values[line++] = {ReadVCounter(), MD::VDP::ReadStatus().horizontal_blanking};
}

void HVTest::VerticalInterrupt()
{
	if (do_sample)
	{
		do_sample = false;

		line = 0;
		MD::VDP::Write(MD::VDP::Register00{.blank_leftmode_8_pixels = false, .enable_horizontal_interrupt = true, .lock_hv_counter = false});
	}
	else
	{
		MD::VDP::Write(MD::VDP::Register00{.blank_leftmode_8_pixels = false, .enable_horizontal_interrupt = false, .lock_hv_counter = false});
	}
}
