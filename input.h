#pragma once
#include <stdio.h>

#include "SDL.h"

class Input {
public:
	int detectButton(int* pos, int status, SDL_Rect rects[7]);
	int handleKey(SDL_Keycode key, SDL_Keymod mod);

private:
	bool isMouseInRect(int* pos, SDL_Rect* rect);

};