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

#include <clownmdsdk.h>

namespace MCD = ClownMDSDK::SubCPU;

void _SP_Init() {}
void _SP_VerticalInterrupt() {}
void _SP_User() {}

void _SP_Main()
{
	MCD::communication_flag_ours = 0x87;
	while (MCD::communication_flag_theirs != 0x87);

	MCD::BIOS::Drive::Initialise({0, 0xFF});

	MCD::BIOS::CDC::Stop();

	for (unsigned int i = 0; i < 100; ++i)
	{
		static constexpr auto WaitForSectorAvailable = []()
		{
			for (unsigned int i = 0; i < 10000; ++i)
				if (MCD::BIOS::CDC::SectorsAvailableForReading())
					return true;

			return false;
		};

		MCD::BIOS::CDROM::ReadN({0, 1});

		if (!WaitForSectorAvailable())
			continue;

		MCD::cdc_mode.device_destination = 2;

		if (!MCD::BIOS::CDC::Read())
			continue;

	#if 0
		static std::array<unsigned short, 0x400> sector_buffer;

	#if 1
		unsigned short *sector_buffer_pointer = sector_buffer.data();

		while (!MCD::cdc_mode.data_set_ready);

		// Read header junk.
		*sector_buffer_pointer = MCD::cdc_host_data;
		*sector_buffer_pointer = MCD::cdc_host_data;

		do
			*sector_buffer_pointer++ = MCD::cdc_host_data;
		while (!MCD::cdc_mode.end_of_data_transfer);
	#else
		unsigned long header;
		void *data_pointer = sector_buffer.data();
		void *header_pointer = &header;
		if (!MCD::BIOS::CDC::Transfer(data_pointer, header_pointer))
			continue;
	#endif

		MCD::BIOS::Music::PlayRepeat(sector_buffer[0] == 0x5345 ? 4 : 5);
		for (;;);
	#endif

		MCD::communication_flag_ours = 0x97;
		while (MCD::communication_flag_theirs != 0x97);

		MCD::BIOS::CDC::Acknowledge();
		break;
	}

	for (;;);
}
