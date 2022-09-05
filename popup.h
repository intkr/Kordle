#pragma once
#include <stdlib.h>

#include <SDL.h>

#include "kordle.h"
#include "graphics.h"
#include "font.h"

class Popup {
public:
	Popup();
	~Popup();
	void drawPopup(Graphics* g, Font* f, Kordle* k, int code);
	void renderPopup(Graphics* g);
	void closeBigPopup();

	SDL_Texture* bigPopTexture; // screen-wide popup
	SDL_Texture* smallPopTexture; // small popup
	int smallPopFrames;
	bool isBigPopOpen;
private:
	void drawSmallPopup(Graphics* g, Font* f, int code);
	void drawBigPopup(Graphics* g, Font* f, Kordle* k, int code);
	void roundSmallPopup(Graphics* g, SDL_Texture* t);

	void renderBigText(Graphics* g, Font* f, int x, int y, int unicode);
	
	// check if it's -1. if so, popup was just triggered; set it to 10
	// for each frame after, decrease it steadily until it reaches 0
	// when it's at 0, do not change it until the popup disappears; set it to -1
	int popAniFrames;

	SDL_Texture* tmpTexture;
	SDL_Surface* tmpSurface;
	SDL_Rect tmpRect;
};