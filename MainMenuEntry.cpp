#include "MainMenuEntry.h"


extern int x, y;
bool hover;
MainMenuEntry::MainMenuEntry() {}
MainMenuEntry::MainMenuEntry(int _x,int _y,bool _hover)
{
	x = _x;
	y = _y;
	hover = _hover;
}

