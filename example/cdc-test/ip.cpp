// Copyright (c) 2024 Clownacy

// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.

// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <array>
#include <variant>

#include <clownmdsdk.h>

#include "../common/control-pad-manager.h"

#include "cdc-test.h"
#include "control-pad.h"
#include "graphics-test.h"
#include "hv-test.h"
#include "main-menu.h"
#include "mode.h"
#include "utility.h"

namespace MD = ClownMDSDK::MainCPU;

#ifdef CD
namespace MCD_RAM = MD::MegaCD::CDBoot;
#else
namespace MCD_RAM = MD::MegaCD::CartridgeBoot;
#endif

static std::variant<MainMenu, CDCTest, GraphicsTest, HVTest> mode;

#ifndef CD
[[noreturn]] static void ErrorTrap()
{
	MD::VDP::CRAM::Fill({7, 0, 0});

	for (;;)
		asm("stop #0x2700");
}

void _BusErrorHandler()
{
	ErrorTrap();
}

void _AddressErrorHandler()
{
	ErrorTrap();
}

void _IllegalInstructionHandler()
{
	ErrorTrap();
}

void _DivisionByZeroHandler()
{
	ErrorTrap();
}

void _CHKHandler()
{
	ErrorTrap();
}

void _TRAPVHandler()
{
	ErrorTrap();
}

void _PrivilegeViolationHandler()
{
	ErrorTrap();
}

void _TraceHandler()
{
	ErrorTrap();
}

void _UnimplementedInstructionLineAHandler()
{
	ErrorTrap();
}

void _UnimplementedInstructionLineFHandler()
{
	ErrorTrap();
}

void _UnassignedHandler()
{
	ErrorTrap();
}

void _UninitialisedInterruptHandler()
{
	ErrorTrap();
}

void _SpuriousInterruptHandler()
{
	ErrorTrap();
}

void _ControllerInterruptHandler()
{

}

void _HorizontalInterruptHandler()
{
	std::visit(
		[](auto &&mode)
		{
			mode.HorizontalInterrupt();
		},
		mode
	);
}

void _TRAP0Handler()
{

}

void _TRAP1Handler()
{

}

void _TRAP2Handler()
{

}

void _TRAP3Handler()
{

}

void _TRAP4Handler()
{

}

void _TRAP5Handler()
{

}

void _TRAP6Handler()
{

}

void _TRAP7Handler()
{

}

void _TRAP8Handler()
{

}

void _TRAP9Handler()
{

}

void _TRAP10Handler()
{

}

void _TRAP11Handler()
{

}

void _TRAP12Handler()
{

}

void _TRAP13Handler()
{

}

void _TRAP14Handler()
{

}

void _TRAP15Handler()
{

}
#endif

#ifdef CD
__attribute__((interrupt)) static void VerticalInterrupt()
#else
void _VerticalInterruptHandler()
#endif
{
//	MD::MegaCD::subcpu.raise_interrupt_level_2 = true;

	control_pad_manager.Update();

	std::visit(
		[](auto &&mode)
		{
			mode.VerticalInterrupt();
		},
		mode
	);
}

void _EntryPoint()
{
	auto vdp_register01 = MD::VDP::Register01{.enable_display = false, .enable_vertical_interrupt = false, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	MD::VDP::Write(vdp_register01);

#ifdef CD
	MD::MegaCD::jump_table.level_6.address = VerticalInterrupt;
#else
//	static constexpr auto subcpu_payload = std::to_array<unsigned char>({
//		#embed "bin/sp.bin"
//	});

//	MCD_RAM::InitialiseSubCPU<unsigned char>(subcpu_payload);

	// Upload the font to VRAM.
	{
		static constexpr auto font = std::to_array<unsigned char>({
			#embed "../common/font.unc"
		});

		MD::Z80::Bus::Lock(
			[&](auto &z80_bus)
			{
				z80_bus.CopyWordsToVDPWithDMA(MD::VDP::RAM::VRAM, ' ' * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL, std::data(font), std::size(font) / sizeof(short));
			}
		);
	}
#endif

	// Write colours to CRAM.
	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, 0);
	MD::VDP::Write(MD::VDP::CRAM::Colour{0, 0, 0});
	MD::VDP::Write(MD::VDP::CRAM::Colour{7, 7, 7});

	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, (16 + 1) * 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{5, 5, 5});

	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, (32 + 1) * 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{2, 2, 7});

	mode.emplace<MainMenu>();

	// Finished setup.
	vdp_register01.enable_display = true;
	vdp_register01.enable_vertical_interrupt = true;
	MD::VDP::Write(vdp_register01);

	MD::M68k::SetInterruptMask(0);

	MD::VDP::VRAM::SetPlaneALocation(VRAM_PLANE_A);

	for (;;)
	{
		// Wait for vertical interrupt.
		asm("stop #0x2000");

		const auto mode_id = std::visit(
			[](auto &&mode)
			{
				return mode.Update();
			},
			mode
		);

		switch (mode_id)
		{
			case ModeID::UNCHANGED:
				break;

			case ModeID::MAIN_MENU:
				mode.emplace<MainMenu>();
				break;

			case ModeID::CDC_TEST:
				mode.emplace<CDCTest>();
				break;

			case ModeID::GRAPHICS_TEST:
				mode.emplace<GraphicsTest>();
				break;

			case ModeID::HV_TEST_H_INT_0:
				mode.emplace<HVTest>(2, 0); // Starts with 0xE1, then counts up from 0.
				break;

			case ModeID::HV_TEST_H_INT_223:
				mode.emplace<HVTest>(2, 223); // Starts with 0xE1, then counts up from 0.
				break;

			case ModeID::HV_TEST_H_INT_224:
				mode.emplace<HVTest>(2, 224); // Starts with 0xE1, then counts up from 0.
				break;

			case ModeID::HV_TEST_H_INT_225:
				mode.emplace<HVTest>(2, 225); // Starts with 0xE1, then counts up from 0.
				break;

			case ModeID::HV_TEST_V_START:
				mode.emplace<HVTest>(20, 0); // Should be 0xE0.
				break;

			case ModeID::HV_TEST_V_END:
				mode.emplace<HVTest>(30, 0); // Should be 0xFF.
				break;

			case ModeID::HV_TEST_H_START:
				mode.emplace<HVTest>(40, 0); // Counts up from 0xFF.
				break;

			case ModeID::HV_TEST_H_END:
				mode.emplace<HVTest>(50, 0); // Counts up from 0xFF.
				break;
		}
	}
}
