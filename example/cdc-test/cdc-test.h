#ifndef CDC_TEST_H
#define CDC_TEST_H

#include "mode.h"

class CDCTest
{
private:
	unsigned int hex_viewer_starting_position = 0;
	unsigned int current_mode = 0;

	void DrawHexViewer();
	void DoEverything();

public:
	CDCTest();
	ModeID Update();
};

#endif // CDC_TEST_H
