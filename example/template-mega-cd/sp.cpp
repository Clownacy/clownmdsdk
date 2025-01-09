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

// Runs once at startup, before anything else.
void _SP_Init()
{
	// Do initialisation here.
}

// Runs once per frame, either 50 or 60 times a second for PAL or NTSC respectively.
void _SP_VerticalInterrupt()
{

}

// Runs in response to the user interrupt, which only occurs when manually triggered by the programmer.
void _SP_User()
{

}

// Run indefinitely; should not return. Handles the bulk of operations.
void _SP_Main()
{
	for (;;)
	{
		// Run main-loop logic here.
	}
}
