#ifndef MODE_H
#define MODE_H

#include <optional>

enum class ModeID
{
	UNCHANGED,
	MAIN_MENU,
	CDC_TEST,
	GRAPHICS_TEST,
	HV_TEST_H_INT,
	HV_TEST_V_START,
	HV_TEST_V_END,
	HV_TEST_H_START,
	HV_TEST_H_END
};

class Mode
{
public:
	virtual ModeID Update() = 0;
};

#endif // MODE_H
