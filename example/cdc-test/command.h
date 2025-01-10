#ifndef COMMAND_H
#define COMMAND_H

#include <utility>

#include "system.h"

enum class Command : unsigned char
{
	NONE = 0,
	REQUEST_WORD_RAM,
	BEGIN_TRANSFER_BIOS,
	BEGIN_TRANSFER_HOST_MAIN_READ_PAST_END,
	BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_DSR,
	BEGIN_TRANSFER_HOST_MAIN_WAIT_FOR_EDT,
	BEGIN_TRANSFER_HOST_SUB_READ_PAST_END,
	BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_DSR,
	BEGIN_TRANSFER_HOST_SUB_WAIT_FOR_EDT,
	BEGIN_TRANSFER_DMA_PCM,
	BEGIN_TRANSFER_DMA_PCM_OFFSET_8,
	BEGIN_TRANSFER_DMA_PRG,
	BEGIN_TRANSFER_DMA_PRG_OFFSET_8,
	BEGIN_TRANSFER_DMA_WORD,
	BEGIN_TRANSFER_DMA_WORD_OFFSET_8,
	END_TRANSFER,
	DO_GRAPHICS_TRANSFORMATION,
};

inline void SubmitSubCPUCommand(const Command command)
{
	static constexpr auto Internal = [](const Command command)
	{
		MD::MegaCD::communication_flag_ours = std::to_underlying(command);
		while (MD::MegaCD::communication_flag_theirs != std::to_underlying(command));
	};

	Internal(command);
	// Send a dummy command so that the same command can be
	// submitted twice in a row without the SUB-CPU ignoring it.
	Internal(Command::NONE);
}

#endif // COMMAND_H
