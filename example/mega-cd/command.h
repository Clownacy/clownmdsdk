#ifndef COMMAND_H
#define COMMAND_H

enum class Command : unsigned char
{
	NONE = 0,
	BEGIN_TRANSFER_BIOS,
	BEGIN_TRANSFER_HOST_MAIN,
	BEGIN_TRANSFER_HOST_SUB,
	END_TRANSFER,
};

#endif // COMMAND_H
