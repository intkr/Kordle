#include "input.h"

int Input::detectClickedButton(int pos[2], int status, SDL_Rect rects[6]) {
	int btnCode = -1;
	switch (status) {
	case 2: // Popup
		if (isMouseInRect(pos, &rects[5])) {
			btnCode = 5;
		}
		break;
	case 1: // Menu
		for (int i = 2; i <= 4; i++) {
			if (isMouseInRect(pos, &rects[i])) {
				btnCode = i;
			}
		}
	default: // None
		for (int i = 0; i <= 1; i++) {
			if (isMouseInRect(pos, &rects[i])) {
				btnCode = i;
			}
		}
	}
	return btnCode;
}

int Input::handleClick(int btn) {
	switch (btn) {
	case 0:
		return 1;
	default:
		// No buttons are pressed
		return -1;
	}
}

bool Input::isMouseInRect(int pos[2], SDL_Rect* rect) {
	return (rect->w + rect->x - pos[0] > 0) && (rect->h + rect->y - pos[1] > 0) && (rect->x - pos[0] < 0) && (rect->y - pos[1] < 0);
}