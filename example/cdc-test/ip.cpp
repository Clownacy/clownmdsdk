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

#ifdef __CLOWNMDSDK_CARTRIDGE__
#define _HORIZONTALINTERRUPTHANDLER ClownMDSDK::M68kInterruptTrampoline
#endif
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

#ifdef __CLOWNMDSDK_CARTRIDGE__
namespace MCD_RAM = MD::MegaCD::CartridgeBoot;
#else
namespace MCD_RAM = MD::MegaCD::CDBoot;
#endif

_HORIZONTALINTERRUPTHANDLER _HorizontalInterruptHandler;

static std::variant<MainMenu, CDCTest, GraphicsTest, HVTest> mode;

#ifdef __CLOWNMDSDK_CARTRIDGE__
[[noreturn]] static void ErrorTrap()
{
	MD::VDP::CRAM::Fill({7, 0, 0});

	for (;;)
		MD::M68k::WaitForInterrupt(7);

	std::unreachable();
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
#endif

#ifdef __CLOWNMDSDK_CARTRIDGE__
void _VerticalInterruptHandler()
#else
static void VerticalInterrupt()
#endif
{
#ifdef __CLOWNMDSDK_CARTRIDGE__
	// We need to do this manually in 'mode 1' cartridge software.
	MD::MegaCD::subcpu.raise_interrupt_level_2 = true;
#endif

	control_pad_manager.Update();

	std::visit(
		[](auto &&mode)
		{
			mode.VerticalInterrupt();
		},
		mode
	);
}

template<typename T, typename... Args>
static void SetMode(Args &&...args)
{
	mode.emplace<T>(std::forward<Args>(args)...);

	const auto &SetHorizontalInterruptHandler = []<auto Callback>()
	{
	#ifdef __CLOWNMDSDK_CARTRIDGE__
		_HorizontalInterruptHandler.SetAddress<Callback>();
	#else
		MD::MegaCD::SetHorizontalInterruptHandler<Callback>();
	#endif
	};

	SetHorizontalInterruptHandler.template operator()<[]()
	{
		// Surely this is a GCC extension? Does the C++ standard actually permit this?
		reinterpret_cast<T&>(mode).HorizontalInterrupt();
	}>();
}

void _EntryPoint()
{
	auto vdp_register01 = MD::VDP::Register01{.enable_display = false, .enable_vertical_interrupt = false, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	MD::VDP::Write(vdp_register01);

	MD::VDP::Write(MD::VDP::Register0C{.enable_h40_cell_mode_1 = false, .enable_shadow_highlight_mode = false, .interlace_mode = 0, .enable_h40_cell_mode_2 = false});

#ifdef __CLOWNMDSDK_CARTRIDGE__
	// Upload Sub-CPU payload.
	{
		static constexpr auto subcpu_payload = std::to_array<unsigned char>({
			#embed "build/sp.kos"
		});

		MCD_RAM::InitialiseSubCPU(std::data(subcpu_payload));
	}

	// Upload font.
	{
		static constexpr auto font_uncompressed = std::to_array<unsigned char>({
			#embed "../common/font.unc"
		});

		static constexpr auto font_compressed = std::to_array<unsigned char>({
			#embed "build/font.kos"
		});

		std::array<unsigned char, std::size(font_uncompressed)> buffer;
		ClownLZSS::KosinskiDecompress(std::begin(font_compressed), std::begin(buffer));

		MD::Z80::Bus::Lock(
			[&](auto &z80_bus)
			{
				z80_bus.CopyWordsToVDPWithDMA(MD::VDP::RAM::VRAM, ' ' * MD::VDP::VRAM::TILE_SIZE_IN_BYTES_NORMAL, std::data(buffer), std::size(buffer) / sizeof(short));
			}
		);
	}
#else
	MD::MegaCD::jump_table.vertical_interrupt.SetAddress<VerticalInterrupt>();
#endif

	// Write colours to CRAM.
	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, 0);
	MD::VDP::Write(MD::VDP::CRAM::Colour{0, 0, 0});
	MD::VDP::Write(MD::VDP::CRAM::Colour{7, 7, 7});

	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, (16 + 1) * 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{5, 5, 5});

	MD::VDP::SendCommand(MD::VDP::RAM::CRAM, MD::VDP::Access::WRITE, (32 + 1) * 2);
	MD::VDP::Write(MD::VDP::CRAM::Colour{2, 2, 7});

	SetMode<MainMenu>();

	// Finished setup.
	vdp_register01.enable_display = true;
	vdp_register01.enable_vertical_interrupt = true;
	MD::VDP::Write(vdp_register01);

	MD::VDP::VRAM::SetPlaneALocation(VRAM_PLANE_A);

	for (;;)
	{
		// Wait for vertical interrupt.
		MD::M68k::WaitForInterrupt(0);

		const auto mode_id = std::visit(
			[](auto &&mode)
			{
				return mode.Update();
			},
			mode
		);

		if (mode_id != ModeID::UNCHANGED)
		{
			// Disable interrupts, so that 'mode' is not visited whilst it is still being emplaced.
			MD::M68k::DisableInterruptsTemporarily(
				[&]()
				{
					switch (mode_id)
					{
						case ModeID::UNCHANGED:
							break;

						case ModeID::MAIN_MENU:
							SetMode<MainMenu>();
							break;

						case ModeID::CDC_TEST:
							SetMode<CDCTest>();
							break;

						case ModeID::GRAPHICS_TEST:
							SetMode<GraphicsTest>();
							break;

						case ModeID::HV_TEST_H_INT_0:
							SetMode<HVTest>(2, 0); // Starts with 0xE1, then counts up from 0.
							break;

						case ModeID::HV_TEST_H_INT_1:
							SetMode<HVTest>(2, 1); // Starts with 0xE1, then counts up from 0.
							break;

						case ModeID::HV_TEST_H_INT_223:
							SetMode<HVTest>(2, 223); // Starts with 0xE1, then counts up from 0.
							break;

						case ModeID::HV_TEST_H_INT_224:
							SetMode<HVTest>(2, 224); // Starts with 0xE1, then counts up from 0.
							break;

						case ModeID::HV_TEST_H_INT_225:
							SetMode<HVTest>(2, 225); // Starts with 0xE1, then counts up from 0.
							break;

						case ModeID::HV_TEST_V_START:
							SetMode<HVTest>(20, 0); // Should be 0xE0.
							break;

						case ModeID::HV_TEST_V_END:
							SetMode<HVTest>(30, 0); // Should be 0xFF.
							break;

						case ModeID::HV_TEST_H_START:
							SetMode<HVTest>(40, 0); // Counts up from 0xFF.
							break;

						case ModeID::HV_TEST_H_END:
							SetMode<HVTest>(50, 0); // Counts up from 0xFF.
							break;
					}
				}
			);
		}
	}
}
