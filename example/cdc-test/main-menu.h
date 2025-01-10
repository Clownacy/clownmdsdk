#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "mode.h"

class MainMenu
{
private:
	unsigned int option_index = 0;

	void DoEverything();

public:
	MainMenu();
	ModeID Update();
};

#endif // MAIN_MENU_H
