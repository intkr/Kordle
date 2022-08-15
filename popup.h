#pragma once
#include <SDL.h>

#include "graphics.h"
#include "font.h"

class Popup {
public:
	Popup();
	~Popup();
	void drawPopup(Graphics* g, Font* f, int code);
	void renderPopup(Graphics* g);

	SDL_Texture* bigPopTexture; // screen-wide popup
	SDL_Texture* smallPopTexture; // small popup
	int smallPopFrames;
	int bigPopFrames;
private:
	void drawSmallPopup(Graphics* g, Font* f, int code);
	void drawBigPopup(Graphics* g, Font* f, int code);
	
	SDL_Texture* tmpTexture;
	SDL_Rect tmpRect;
};