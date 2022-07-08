#pragma once
#include <stdio.h>

#include "SDL.h"

class Input {
public:
	// Checks which button is clicked, and returns the corresponding integer code.
	// Refer to notes.txt for details.
	int detectButton(int pos[2], int status, SDL_Rect rects[6]);

	// Handles activity based on currently clicked button.
	// Returns an integer for usage within other classes.
	// -1 if no buttons are pressed, refer to notes.txt for other values.
	int handleClick(int btn);

private:
	bool isMouseInRect(int pos[2], SDL_Rect* rect);

};