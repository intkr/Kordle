#pragma once
#include <stdlib.h>

#include <SDL.h>

#include "kordle.h"
#include "graphics.h"
#include "font.h"
#include "controls.h"

class Popup {
public:
	Popup(Graphics* g, Settings* s);
	~Popup();
	void drawPopup(Graphics* g, Font* f, Kordle* k, int code);
	void renderPopup(Graphics* g, Font* f);
	void closeBigPopup();
	void reset();
	void reset2(Graphics* g, Font* f, Kordle* k);

	void updateSwitch(int n);

	Controls* _controls;
	SDL_Texture* bigPopTexture; // screen-wide popup
	SDL_Texture* smallPopTexture; // small popup
	int smallPopFrames;
	int openedBigPop;
private:
	void drawSmallPopup(Graphics* g, Font* f, int code);
	void drawBigPopup(Graphics* g, Font* f, Kordle* k, int code);
	void roundTexture(Graphics* g, SDL_Texture* t, int base);

	void renderBigText(Graphics* g, Font* f, int x, int y, int unicode);
	void renderUNICODE(Graphics* g, Font* f, int x, int y, Uint16* text);
	void renderUTF8(Graphics* g, Font* f, int x, int y, const char* text);

	void drawTimer(Graphics* g, Font* f);
	
	// check if it's -1. if so, popup was just triggered; set it to 10
	// for each frame after, decrease it steadily until it reaches 0
	// when it's at 0, do not change it until the popup disappears; set it to -1
	int popAniFrames;

	SDL_Texture* tmpTexture;
	SDL_Surface* tmpSurface;
	SDL_Rect tmpRect;

	Uint8 smallPopColor;
	Uint8 bigPopColor;
};