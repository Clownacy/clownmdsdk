#ifndef GRAPHICS_TEST_H
#define GRAPHICS_TEST_H

#include "mode.h"

class GraphicsTest
{
public:
	GraphicsTest();
	ModeID Update();
	void HorizontalInterrupt() {}
	void VerticalInterrupt() {}
};

#endif // GRAPHICS_TEST_H
