#pragma once
#include "SDL.h"

#include "graphics.h"

class Switch {
public:
	Switch();
	~Switch();

	SDL_Texture* getSwitchTexture();

	bool _side; // Left(False), Right(True)
	int aniFrame; // (FPS / 4) frames. positive = right, negative = left, 0 = none

	SDL_Texture* _texture;
	SDL_Rect dstRect;
};

class Controls {
public:
	Controls(Graphics* g);
	~Controls();
	
	void reset(Graphics* g);
	int detectSwitch(int* pos);
	void drawSwitch(Graphics* g);

	Switch* switches;
	SDL_Texture* switchBodyTexture;
	SDL_Texture* switchHeadTexture;

private:
	void drawTextures(Graphics* g, int* _x, int* _y, bool* side);
	bool isMouseInRect(int* pos, SDL_Rect* rect);

	SDL_Rect tmpRect;
	Uint8 bgColor;
	Uint8 bodyColor;
	int _n; // number of buttons
};