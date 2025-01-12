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

static constexpr std::size_t SECTOR_BUFFER_LENGTH = 0x408;

static const std::span<unsigned short, SECTOR_BUFFER_LENGTH> sector_buffer(MCD::word_ram_2m<unsigned short>.data(), SECTOR_BUFFER_LENGTH);

alignas(8) static std::array<unsigned short, SECTOR_BUFFER_LENGTH> prg_ram_sector_buffer;

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

			case Command::REQUEST_WORD_RAM:
				MCD::GiveWordRAMToMainCPU();
				break;

			case Command::BEGIN_TRANSFER_BIOS:
			case Command::BEGIN_TRANSFER_HOST_MAIN_READ_PAST_END:
			case Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_DSR:
			case Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_EDT:
			case Command::BEGIN_TRANSFER_HOST_SUB_READ_PAST_END:
			case Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_DSR:
			case Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_EDT:
			case Command::BEGIN_TRANSFER_DMA_PCM:
			case Command::BEGIN_TRANSFER_DMA_PCM_OFFSET_8:
			case Command::BEGIN_TRANSFER_DMA_PRG:
			case Command::BEGIN_TRANSFER_DMA_PRG_OFFSET_8:
			case Command::BEGIN_TRANSFER_DMA_WORD:
			case Command::BEGIN_TRANSFER_DMA_WORD_OFFSET_8:
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

					switch (command)
					{
						default:
							break;

						case Command::BEGIN_TRANSFER_HOST_MAIN_READ_PAST_END:
						case Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_DSR:
						case Command::BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_EDT:
							MCD::CDC::mode.device_destination = MCD::CDC::DeviceDestination::MAIN_CPU;
							break;

						case Command::BEGIN_TRANSFER_BIOS:
						case Command::BEGIN_TRANSFER_HOST_SUB_READ_PAST_END:
						case Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_DSR:
						case Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_EDT:
							MCD::CDC::mode.device_destination = MCD::CDC::DeviceDestination::SUB_CPU;
							break;

						case Command::BEGIN_TRANSFER_DMA_PCM:
							std::fill(std::begin(MCD::PCM::ram_window), std::end(MCD::PCM::ram_window), 0);
							MCD::CDC::mode.device_destination = MCD::CDC::DeviceDestination::PCM_RAM;
							break;

						case Command::BEGIN_TRANSFER_DMA_PCM_OFFSET_8:
							std::fill(std::begin(MCD::PCM::ram_window), std::end(MCD::PCM::ram_window), 0);
							MCD::CDC::mode.device_destination = MCD::CDC::DeviceDestination::PCM_RAM;
							MCD::CDC::SetDMAAddress(8);
							break;

						case Command::BEGIN_TRANSFER_DMA_PRG:
							std::fill(std::begin(prg_ram_sector_buffer), std::end(prg_ram_sector_buffer), 0);
							MCD::CDC::mode.device_destination = MCD::CDC::DeviceDestination::PRG_RAM;
							MCD::CDC::SetDMAAddress(reinterpret_cast<std::uintptr_t>(&prg_ram_sector_buffer));
							break;

						case Command::BEGIN_TRANSFER_DMA_PRG_OFFSET_8:
							std::fill(std::begin(prg_ram_sector_buffer), std::end(prg_ram_sector_buffer), 0);
							MCD::CDC::mode.device_destination = MCD::CDC::DeviceDestination::PRG_RAM;
							MCD::CDC::SetDMAAddress(reinterpret_cast<std::uintptr_t>(&prg_ram_sector_buffer) + 8);
							break;

						case Command::BEGIN_TRANSFER_DMA_WORD:
							std::fill(std::begin(sector_buffer), std::end(sector_buffer), 0);
							MCD::CDC::mode.device_destination = MCD::CDC::DeviceDestination::WORD_RAM;
							break;

						case Command::BEGIN_TRANSFER_DMA_WORD_OFFSET_8:
							std::fill(std::begin(sector_buffer), std::end(sector_buffer), 0);
							MCD::CDC::mode.device_destination = MCD::CDC::DeviceDestination::WORD_RAM;
							MCD::CDC::SetDMAAddress(8);
							break;
					}

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

						case Command::BEGIN_TRANSFER_HOST_SUB_READ_PAST_END:
						case Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_DSR:
						case Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_EDT:
						{
							auto *sector_buffer_pointer = sector_buffer.data();

							while (!MCD::CDC::mode.data_set_ready);

							// Read header junk.
							*sector_buffer_pointer = MCD::CDC::host_data;
							*sector_buffer_pointer = MCD::CDC::host_data;

							switch (command)
							{
								default:
									break;

								case Command::BEGIN_TRANSFER_HOST_SUB_READ_PAST_END:
									for (auto &word : sector_buffer)
										word = MCD::CDC::host_data;
									break;

								case Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_DSR:
									while (MCD::CDC::mode.data_set_ready)
										*sector_buffer_pointer++ = MCD::CDC::host_data;
									break;

								case Command::BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_EDT:
									while (!MCD::CDC::mode.end_of_data_transfer)
										*sector_buffer_pointer++ = MCD::CDC::host_data;
									break;
							}

							break;
						}

						case Command::BEGIN_TRANSFER_DMA_PCM:
						case Command::BEGIN_TRANSFER_DMA_PCM_OFFSET_8:
							while (!MCD::CDC::mode.end_of_data_transfer);
							for (std::size_t i = 0; i < SECTOR_BUFFER_LENGTH; ++i)
								sector_buffer[i] = (MCD::PCM::ram_window[i * 2 + 0] << 8) | (MCD::PCM::ram_window[i * 2 + 1] & 0xFF);
							break;

						case Command::BEGIN_TRANSFER_DMA_PRG:
						case Command::BEGIN_TRANSFER_DMA_PRG_OFFSET_8:
							while (!MCD::CDC::mode.end_of_data_transfer);
							std::copy(std::begin(prg_ram_sector_buffer), std::end(prg_ram_sector_buffer), std::begin(sector_buffer));
							break;

						case Command::BEGIN_TRANSFER_DMA_WORD:
						case Command::BEGIN_TRANSFER_DMA_WORD_OFFSET_8:
							while (!MCD::CDC::mode.end_of_data_transfer);
							break;
					}

					MCD::GiveWordRAMToMainCPU();
					break;
				}

				break;

			case Command::END_TRANSFER:
				MCD::BIOS::CDC::Acknowledge();
				break;

			case Command::DO_GRAPHICS_TRANSFORMATION:
			{
				const unsigned int width = 7;
				const unsigned int height = 3;

				MCD::stamp_data_size.large_stamp_map = false;
				MCD::stamp_data_size.large_stamps = true;
				MCD::stamp_data_size.repeat = true;

				const std::size_t stamp_map_offset = 0x10000;
				const std::size_t image_buffer_offset = 0x20000;

				MCD::stamp_map_base_address = stamp_map_offset / 4;
				MCD::image_buffer_vertical_cell_size = height - 1;
				MCD::image_buffer_start_address = image_buffer_offset / 4;
				MCD::image_buffer_offset.y = 0;
				MCD::image_buffer_offset.x = 4;
				MCD::image_buffer_horizontal_dot_size = width * 8;
				MCD::image_buffer_vertical_dot_size = height * 8 + 4;

				for (unsigned int y = 0; y < 2; ++y)
				{
					for (unsigned int x = 0; x < 2; ++x)
					{
						MCD::word_ram_2m<unsigned short>[stamp_map_offset / 2 + y * (256 / 32) + x] = (1 + (y * 2 + x)) * 4;
					}
				}

				const std::size_t trace_vector_offset = 0x30000;

				for (unsigned int i = 0; i < MCD::image_buffer_vertical_dot_size; ++i)
				{
					MCD::word_ram_2m<unsigned short>[trace_vector_offset / 2 + i * 4 + 0] = 0;
					MCD::word_ram_2m<unsigned short>[trace_vector_offset / 2 + i * 4 + 1] = i << 3;
					MCD::word_ram_2m<unsigned short>[trace_vector_offset / 2 + i * 4 + 2] = 1 << 11;
					MCD::word_ram_2m<unsigned short>[trace_vector_offset / 2 + i * 4 + 3] = (0 << 11) + 0;
				}

				MCD::trace_vector_base_address = trace_vector_offset / 4;

				while (MCD::stamp_data_size.graphics_operation_in_progress);

				MCD::GiveWordRAMToMainCPU();

				break;
			}
		}

		MCD::communication_flag_ours = command_value;
	}
}
