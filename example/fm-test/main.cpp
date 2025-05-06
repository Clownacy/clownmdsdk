// Copyright (c) 2025 Clownacy

// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted.

// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <clownmdsdk.h>

#include "../common/control-pad-manager.h"

static ControlPadManager<1> control_pad_manager;

// M68000 exception handlers.
void _BusErrorHandler()
{

}

void _AddressErrorHandler()
{

}

void _IllegalInstructionHandler()
{

}

void _DivisionByZeroHandler()
{

}

void _CHKHandler()
{

}

void _TRAPVHandler()
{

}

void _PrivilegeViolationHandler()
{

}

void _TraceHandler()
{

}

void _UnimplementedInstructionLineAHandler()
{

}

void _UnimplementedInstructionLineFHandler()
{

}

void _UnassignedHandler()
{

}

void _UninitialisedInterruptHandler()
{

}

void _SpuriousInterruptHandler()
{

}

void _ControllerInterruptHandler()
{

}

void _HorizontalInterruptHandler()
{

}

// Runs once per frame, either 50 or 60 times a second for PAL or NTSC respectively.
void _VerticalInterruptHandler()
{
	control_pad_manager.Update();
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

struct Operator
{
	unsigned char detune, multiplier, total_level, key_scale, attack_rate, decay_rate, sustain_rate, release_rate, sustain_level;
	bool amplitude_modulation;
	bool ssgeg_enabled, ssgeg_attack, ssgeg_alternate, ssgeg_hold;
};

struct Instrument
{
	unsigned short frequency;
	unsigned char feedback, algorithm;
	bool pan_left, pan_right;
	unsigned char amplitude_modulation_sensitivity, phase_modulation_sensitivity;
	std::array<Operator, 4> operators;
};

static void Upload(ClownMDSDK::MainCPU::Z80::Bus &z80_bus, const unsigned int slot_index, const unsigned int operator_index, const Operator &op)
{
	const auto register_bits = operator_index << 2 | slot_index << 0;

	z80_bus.WriteFMII(register_bits | 0x30, op.detune << 4 | op.multiplier << 0);
	z80_bus.WriteFMII(register_bits | 0x40, op.total_level);
	z80_bus.WriteFMII(register_bits | 0x50, op.key_scale << 6 | op.attack_rate << 0);
	z80_bus.WriteFMII(register_bits | 0x60, op.amplitude_modulation << 7 | op.decay_rate << 0);
	z80_bus.WriteFMII(register_bits | 0x70, op.sustain_rate);
	z80_bus.WriteFMII(register_bits | 0x80, op.sustain_level << 4 | op.release_rate << 0);
	z80_bus.WriteFMII(register_bits | 0x90, op.ssgeg_enabled << 3 | op.ssgeg_attack << 2 | op.ssgeg_alternate << 1 | op.ssgeg_hold << 0);
}

static void Upload(ClownMDSDK::MainCPU::Z80::Bus &z80_bus, const unsigned int slot_index, const Instrument &instrument)
{
	z80_bus.WriteFMII(slot_index | 0xA4, instrument.frequency >> 8);
	z80_bus.WriteFMII(slot_index | 0xA0, instrument.frequency & 0xFF);

	z80_bus.WriteFMII(slot_index | 0xB0, instrument.feedback << 3 | instrument.algorithm << 0);

	z80_bus.WriteFMII(slot_index | 0xB4, instrument.pan_left << 7 | instrument.pan_right << 6 | instrument.amplitude_modulation_sensitivity << 4 | instrument.phase_modulation_sensitivity << 0);

	for (unsigned int i = 0; i < instrument.operators.size(); ++i)
		Upload(z80_bus, slot_index, i, instrument.operators[i]);

//	z80_bus.WriteFMII(0x27, 0x40); // FM3 mode.
}

// Run indefinitely; should not return. Handles the bulk of operations.
void _EntryPoint()
{
	// Define the instrument.
	static constexpr Instrument instrument = {
		.frequency = 0x1B2A,
		.feedback = 4, .algorithm = 4,
		.pan_left = true, .pan_right = true,
		.amplitude_modulation_sensitivity = 3, .phase_modulation_sensitivity = 0,
		.operators = {{
			{
				.detune = 0, .multiplier = 11, .total_level = 0x65, .key_scale = 0, .attack_rate = 0x1F, .decay_rate = 0x1C, .sustain_rate = 0x00, .release_rate = 0xF, .sustain_level = 0xF,
				.amplitude_modulation = false,
				.ssgeg_enabled = false, .ssgeg_attack = false, .ssgeg_alternate = false, .ssgeg_hold = false,
			},
			{
				.detune = 0, .multiplier = 15, .total_level = 0x19, .key_scale = 0, .attack_rate = 0x1F, .decay_rate = 0x9, .sustain_rate = 0x1F, .release_rate = 0xF, .sustain_level = 0xF,
				.amplitude_modulation = false,
				.ssgeg_enabled = false, .ssgeg_attack = false, .ssgeg_alternate = false, .ssgeg_hold = false,
			},
			{
				.detune = 0, .multiplier = 15, .total_level = 0x15, .key_scale = 3, .attack_rate = 0x1F, .decay_rate = 0x10, .sustain_rate = 0x1F, .release_rate = 0xF, .sustain_level = 0xF,
				.amplitude_modulation = false,
				.ssgeg_enabled = true, .ssgeg_attack = false, .ssgeg_alternate = true, .ssgeg_hold = false,
			},
			{
				.detune = 0, .multiplier = 15, .total_level = 0x2A, .key_scale = 3, .attack_rate = 0x13, .decay_rate = 0xE, .sustain_rate = 0x1F, .release_rate = 0xF, .sustain_level = 0xF,
				.amplitude_modulation = true,
				.ssgeg_enabled = true, .ssgeg_attack = false, .ssgeg_alternate = false, .ssgeg_hold = false,
			},
		}}
	};

	// Upload the instrument.
	{
		ClownMDSDK::MainCPU::Z80::Bus z80_bus;
		Upload(z80_bus, 2, instrument);
	}

	auto vdp_register01 = ClownMDSDK::MainCPU::VDP::Register01{.enable_display = true, .enable_vertical_interrupt = true, .enable_dma_transfer = true, .enable_v30_cell_mode = false, .enable_mega_drive_mode = true};
	ClownMDSDK::MainCPU::VDP::Write(vdp_register01);

	control_pad_manager.Update();

	for (;;)
	{
		// Control the instrument's 'key-on' with the 'A' button.
		{
			ClownMDSDK::MainCPU::Z80::Bus z80_bus;
			z80_bus.WriteFMI(0x28, control_pad_manager.GetControlPads()[0].held.a ? 0xF6 : 0x06);
		}

		// Sleep until the next frame.
		asm("stop #0x2300");
	}
}
