#include "input.h"

// Figures out which button was pressed based on current mouse position.
// Hitboxes of buttons are taken from menuRect (graphics.h)
int Input::detectButton(int* pos, int status, SDL_Rect rects[7]) {
	int btnCode = -1;
	switch (status) {
	case 2: // Popup
		if (isMouseInRect(pos, &rects[6])) {
			btnCode = 6;
		}
		break;
	case 1: // Menu
		for (int i = 2; i <= 4; i++) {
			if (isMouseInRect(pos, &rects[i])) {
				btnCode = i;
			}
		}
		if (btnCode != -1) break;
	default: // None
		for (int i = 0; i <= 1; i++) {
			if (isMouseInRect(pos, &rects[i])) {
				btnCode = i;
			}
		}
		if (isMouseInRect(pos, &rects[5])) {
			btnCode = 5;
		}
	}
	return btnCode;
}

// Returns an arbitrary integer based on button presses.
// Refer to notes.txt for more information on return values.
int Input::handleClick(int btn) {
	switch (btn) {
	case 0:
		return 1;
	case 1:
		return 4;
	case 5:
		return 2;
	default:
		// No buttons are pressed
		return -1;
	}
}

bool Input::isMouseInRect(int* pos, SDL_Rect* rect) {
	return (rect->w + rect->x - pos[0] > 0) && (rect->h + rect->y - pos[1] > 0) && (rect->x - pos[0] < 0) && (rect->y - pos[1] < 0);
}