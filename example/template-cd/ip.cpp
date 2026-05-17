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

static void HorizontalInterrupt()
{

}

// Runs once per frame, either 50 or 60 times a second for PAL or NTSC respectively.
static void VerticalInterrupt()
{
	// There is no need to manually raise the Sub-CPU's vertical interrupt here,
	// as 'vertical_interrupt.SetAddress' ensures that this is done automatically.
}

// Run indefinitely; should not return. Handles the bulk of operations.
void _EntryPoint()
{
	// Register interrupt handlers.
	// 'jump_table.horizontal_interrupt.SetAddress' could be used for this instead,
	// but 'SetHorizontalInterruptHandler' is more efficient due to bypassing a jump instruction.
	ClownMDSDK::MainCPU::MegaCD::SetHorizontalInterruptHandler<HorizontalInterrupt>();
	ClownMDSDK::MainCPU::MegaCD::jump_table.vertical_interrupt.SetAddress<VerticalInterrupt>();

	// Do initialisation here.

	for (;;)
	{
		// Run main-loop logic here.
	}
}
