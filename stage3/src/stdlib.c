#include <stdlib.h>

static unsigned long RepeatByteToUnsignedLong(const int ch)
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

__attribute__((visibility("default"))) int abs(const int i)
{
	return i < 0 ? -i : i;
}

__attribute__((visibility("default"))) void* memcpy(void* const dest, const void* const src, const size_t count)
{
	const size_t total_longs = count / sizeof(unsigned long);
	const size_t remainder_bytes = count % sizeof(unsigned long);

	unsigned long *source_longs = (unsigned long*)src;
	unsigned char *source_bytes = (unsigned char*)(source_longs + total_longs);

	unsigned long *destination_longs = (unsigned long*)dest;
	const unsigned long* const destination_longs_end = destination_longs + total_longs;
	unsigned char *destination_bytes = (unsigned char*)destination_longs_end;
	const unsigned char* const destination_bytes_end = destination_bytes + remainder_bytes;

	for (; destination_longs < destination_longs_end; ++destination_longs)
		*destination_longs = *source_longs++;

	for (; destination_bytes < destination_bytes_end; ++destination_bytes)
		*destination_bytes = *source_bytes++;

	return dest;
}

__attribute__((visibility("default"))) void* memset(void* const dest, const int ch, const size_t count)
{
	const unsigned long ch_long = RepeatByteToUnsignedLong(ch);
	const size_t total_longs = count / sizeof(unsigned long);
	const size_t remainder_bytes = count % sizeof(unsigned long);

	unsigned long *destination_longs = (unsigned long*)dest;
	const unsigned long* const destination_longs_end = destination_longs + total_longs;
	unsigned char *destination_bytes = (unsigned char*)destination_longs_end;
	const unsigned char* const destination_bytes_end = destination_bytes + remainder_bytes;

	for (; destination_longs < destination_longs_end; ++destination_longs)
		*destination_longs = ch_long;

	for (; destination_bytes < destination_bytes_end; ++destination_bytes)
		*destination_bytes = ch;

	return dest;
}
