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

#include <atomic>
#include <span>
#include <utility>

#include <clownmdsdk.h>

#include "command.h"

namespace MCD = ClownMDSDK::SubCPU;

void _SP_Init() {}
void _SP_VerticalInterrupt() {}
void _SP_User() {}

static const std::span<std::atomic<unsigned short>, 0x400> sector_buffer(MCD::word_ram_2m<unsigned short>.data(), 0x400);

void _SP_Main()
{
	MCD::BIOS::Drive::Initialise({0, 0xFF});

	MCD::memory_mode.word_ram_1m_mode = false;

	for (;;)
	{
		while (MCD::communication_flag_ours == MCD::communication_flag_theirs);

		const auto command_value = MCD::communication_flag_theirs.load();
		const auto command = static_cast<Command>(command_value);

		switch (command)
		{
			case Command::NONE:
				break;

			case Command::BEGIN_TRANSFER_BIOS:
			case Command::BEGIN_TRANSFER_HOST_MAIN:
			case Command::BEGIN_TRANSFER_HOST_SUB:
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

					MCD::cdc_mode.device_destination = command == Command::BEGIN_TRANSFER_HOST_MAIN ? 2 : 3;

					if (!MCD::BIOS::CDC::Read())
						continue;

					switch (command)
					{
						default:
							break;

						case Command::BEGIN_TRANSFER_BIOS:
						{
							unsigned long header;
							void *data_pointer = sector_buffer.data();
							void *header_pointer = &header;
							if (!MCD::BIOS::CDC::Transfer(data_pointer, header_pointer))
								continue;
							break;
						}

						case Command::BEGIN_TRANSFER_HOST_SUB:
						{
							auto *sector_buffer_pointer = sector_buffer.data();

							while (!MCD::cdc_mode.data_set_ready);

							// Read header junk.
							*sector_buffer_pointer = MCD::cdc_host_data;
							*sector_buffer_pointer = MCD::cdc_host_data;

							do
								*sector_buffer_pointer++ = MCD::cdc_host_data;
							while (!MCD::cdc_mode.end_of_data_transfer);

							break;
						}
					}

					MCD::GiveWordRAMToMainCPU();
					break;
				}

				break;

			case Command::END_TRANSFER:
				MCD::BIOS::CDC::Acknowledge();
				break;
		}

		MCD::communication_flag_ours = command_value;
	}
}
