#ifndef MODE_H
#define MODE_H

#include <optional>

enum class ModeID
{
	UNCHANGED,
	MAIN_MENU,
	CDC_TEST,
	GRAPHICS_TEST
};

class Mode
{
public:
	virtual ModeID Update() = 0;
};

#endif // MODE_H
