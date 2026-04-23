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
