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

#include <stdint.h>
#include <string.h>

static unsigned long _RepeatByteToUnsignedLong(const int ch)
{
	unsigned long value = 0;

	unsigned int i;

	for (i = 0; i < sizeof(unsigned long); ++i)
	{
		value <<= 8;
		value |= ch;
	}

	return value;
}

void* memcpy(void* const dest, const void* const src, size_t count)
{
	unsigned char *source = (unsigned char*)src;
	unsigned char *destination = (unsigned char*)dest;

	if (count == 0)
		return dest;

	// Slow path where one is word-aligned but the other isn't.
	if ((((uintptr_t)destination ^ (uintptr_t)source) & 1) != 0)
	{
		size_t count_div_10 = count / 0x10;

		asm volatile(
				"jmp	1f(%%pc,%3.w)\n"
			"0:\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"	move.b	(%2)+,(%0)+\n"
			"1:\n"
			"	dbf	%1,0b\n"
			"	clr.w	%1\n"
			"	subq.l	#1,%1\n"
			"	bcc.s	0b\n"
			: "+a" (destination), "+d" (count_div_10), "+a" (source)
			: "da" (-((count % 0x10) * 2))
			: "cc"
		);

		return dest;
	}

	// Process a single byte to put us on an even address, if necessary.
	if (((uintptr_t)destination & 1) != 0)
	{
		asm volatile(
			"move.b	(%1)+,(%0)+\n"
			: "+a" (destination), "+a" (source)
			:
			: "cc"
		);

		--count;
	}

	if (count >= 4)
	{
		// Process the bulk of the data as a series of longwords.
		size_t count_div_10 = count / 0x10;

		asm volatile(
				"jmp	1f(%%pc,%3.w)\n"
			"0:\n"
			"	move.l	(%2)+,(%0)+\n"
			"	move.l	(%2)+,(%0)+\n"
			"	move.l	(%2)+,(%0)+\n"
			"	move.l	(%2)+,(%0)+\n"
			"1:\n"
			"	dbf	%1,0b\n"
			"	clr.w	%1\n"
			"	subq.l	#1,%1\n"
			"	bcc.s	0b\n"
			: "+a" (destination), "+d" (count_div_10), "+a" (source)
			: "da" (-((count % 0x10 / 4) * 2))
			: "cc"
		);
	}

	// Process any trailing bytes.
	if ((count & 2) != 0)
	{
		asm volatile(
			"move.w	(%1)+,(%0)+\n"
			: "+a" (destination), "+a" (source)
			:
			: "cc"
		);
	}

	if ((count & 1) != 0)
	{
		asm volatile(
			"move.b	(%1)+,(%0)+\n"
			: "+a" (destination), "+a" (source)
			:
			: "cc"
		);
	}

	// And we're done!
	return dest;
}

void* memmove(void* const dest, const void* const src, size_t count)
{
	unsigned char *source = (unsigned char*)src;
	unsigned char *destination = (unsigned char*)dest;
	size_t count_div_10 = count / 0x10;

	if (count == 0)
		return dest;

	asm volatile(
			"jmp	1f(%%pc,%3.w)\n"
		"0:\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"	move.b	(%2)+,(%0)+\n"
		"1:\n"
		"	dbf	%1,0b\n"
		"	clr.w	%1\n"
		"	subq.l	#1,%1\n"
		"	bcc.s	0b\n"
		: "+a" (destination), "+d" (count_div_10), "+a" (source)
		: "da" (-((count % 0x10) * 2))
		: "cc"
	);

	return dest;
}

void* memset(void* const dest, const int ch, size_t count)
{
	unsigned char *destination = (unsigned char*)dest;

	if (count == 0)
		return dest;

	const unsigned long value = _RepeatByteToUnsignedLong(ch);

	// Process a single byte to put us on an even address, if necessary.
	if (((uintptr_t)destination & 1) != 0)
	{
		asm volatile(
			"move.b	%1,(%0)+\n"
			: "+a" (destination)
			: "d" (value)
			: "cc"
		);

		--count;
	}

	if (count >= 4)
	{
		// Process the bulk of the data as a series of longwords.
		size_t count_div_10 = count / 0x10;

		asm volatile(
				"jmp	1f(%%pc,%2.w)\n"
			"0:\n"
			"	move.l	%3,(%0)+\n"
			"	move.l	%3,(%0)+\n"
			"	move.l	%3,(%0)+\n"
			"	move.l	%3,(%0)+\n"
			"1:\n"
			"	dbf	%1,0b\n"
			"	clr.w	%1\n"
			"	subq.l	#1,%1\n"
			"	bcc.s	0b\n"
			: "+a" (destination), "+d" (count_div_10)
			: "da" (-((count % 0x10 / 4) * 2)), "d" (ch)
			: "cc"
		);
	}

	// Process any trailing bytes.
	if ((count & 2) != 0)
	{
		asm volatile(
			"move.w	%1,(%0)+\n"
			: "+a" (destination)
			: "d" (value)
			: "cc"
		);
	}

	if ((count & 1) != 0)
	{
		asm volatile(
			"move.b	%1,(%0)+\n"
			: "+a" (destination)
			: "d" (value)
			: "cc"
		);
	}

	// And we're done!
	return dest;
}
