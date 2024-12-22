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

using namespace ClownMDSDK::SubCPU;

void _SP_Init() {}
void _SP_VerticalInterrupt() {}
void _SP_User() {}

void _SP_Main()
{
	BIOS::Drive::Initialise({0, 0xFF});

	const auto &status = BIOS::Misc::Status();

	std::fill(PCM::ram_window, PCM::ram_window_end, status.led);

	const auto entry = BIOS::Misc::ReadTableOfContents(1);
	std::fill(PCM::ram_window, PCM::ram_window_end, entry.is_rom_track);

	static const unsigned long toc[] = {0x12345678, 0x87654321, 0xFFFFFFFF};
	BIOS::Misc::WriteTableOfContents(toc);

	BIOS::CDROM::ReadN({0, 1});

	cdc_mode.device_destination = 3;

	while (!BIOS::CDC::SectorsAvailableForReading());

	if (!BIOS::CDC::Read())
		std::fill(PCM::ram_window, PCM::ram_window_end, 0xBA);

	unsigned long header;
	unsigned char* const data = reinterpret_cast<unsigned char*>(0x20000);
	void *data_pointer = data;
	void *header_pointer = &header;
	if (!BIOS::CDC::Transfer(data_pointer, header_pointer))
		std::fill(PCM::ram_window, PCM::ram_window_end, 0xBB);

	BIOS::CDC::Acknowledge();

	if (data[0] == 0x53)
		BIOS::Music::PlayRepeat(4);

	for (;;);
}
