#include "input.h"

// Button click detection system.
// Hitboxes are defined in menuRect (in graphics.h).
// Refer to 'Button codes' in notes.txt for details.

extern char lang;
extern bool isDarkMode;

int Input::detectButton(int* pos, int status, SDL_Rect rects[7]) {
	int btnCode = -1;
	switch (status) {
	case 3: // Settings Popup

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

// Returns a number corresponding to keys on the physical keyboard.
// Refer to 'Key codes' in notes.txt for details.
int Input::handleKey(SDL_Keycode key, SDL_Keymod mod) {
	switch (key) {
	// consonants
	case SDLK_r:
		if (mod & KMOD_SHIFT) {
			return 2;
		}
		else {
			return 1;
		}
	case SDLK_s:
		return 3;
	case SDLK_e:
		if (mod & KMOD_SHIFT) {
			return 5;
		}
		else {
			return 4;
		}
	case SDLK_f:
		return 6;
	case SDLK_a:
		return 7;
	case SDLK_q:
		if (mod & KMOD_SHIFT) {
			return 9;
		}
		else {
			return 8;
		}
	case SDLK_t:
		if (mod & KMOD_SHIFT) {
			return 11;
		}
		else {
			return 10;
		}
	case SDLK_d:
		return 12;
	case SDLK_w:
		if (mod & KMOD_SHIFT) {
			return 14;
		}
		else {
			return 13;
		}
	case SDLK_c:
		return 15;
	case SDLK_z:
		return 16;
	case SDLK_x:
		return 17;
	case SDLK_v:
		return 18;
	case SDLK_g:
		return 19;
	// vowels (subtract 20 to get vowel value)
	case SDLK_k:
		return 20;
	case SDLK_o:
		if (mod & KMOD_SHIFT) {
			return 23;
		}
		else {
			return 21;
		}
	case SDLK_i:
		return 22;
	case SDLK_j:
		return 24;
	case SDLK_p:
		if (mod & KMOD_SHIFT) {
			return 27;
		}
		else {
			return 25;
		}
	case SDLK_u:
		return 26;
	case SDLK_h:
		return 28;
	case SDLK_y:
		return 32;
	case SDLK_n:
		return 33;
	case SDLK_b:
		return 37;
	case SDLK_m:
		return 38;
	case SDLK_l:
		return 40;
	// other keys
	case SDLK_BACKSPACE:
		return 98;
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
		return 99;
	case SDLK_ESCAPE:
		return 100;
	case SDLK_1:
		return 101;
	case SDLK_2:
		return 102;
	case SDLK_3:
		return 103;
	default:
		return -1;
	}
}

bool Input::isMouseInRect(int* pos, SDL_Rect* rect) {
	return (rect->w + rect->x - pos[0] > 0) && (rect->h + rect->y - pos[1] > 0) && (rect->x - pos[0] < 0) && (rect->y - pos[1] < 0);
}