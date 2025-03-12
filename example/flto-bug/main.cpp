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

#include <array>

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

void _Level1InterruptHandler()
{

}

void _Level2InterruptHandler()
{

}

void _Level3InterruptHandler()
{

}

void _Level4InterruptHandler()
{

}

void _Level5InterruptHandler()
{

}

void _Level6InterruptHandler()
{

}

void _Level7InterruptHandler()
{

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

void _EntryPoint()
{
	// https://github.com/Clownacy/clownmdsdk/issues/3
	// This generates a hidden call to `memset`. A strange issue with GCC's linker would cause this to call address 0.
	// This issue was solved by not compiling `libc.a` with `-flto`.
	// I'll keep this test here though in case I ever want to try using `-flto` for `libc.a` again.
	constexpr std::array<unsigned short, 32> pTestData = { 0 };

	// This is needed so that `pTestData` is not optimised-away.
	ClownMDSDK::MainCPU::VDP::CopyWordsWithoutDMA(ClownMDSDK::MainCPU::VDP::RAM::VRAM, 0, std::data(pTestData), std::size(pTestData));

	for (;;);
}
