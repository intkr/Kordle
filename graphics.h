#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

#include "settings.h"

class Graphics {
public:
	Graphics(Settings* s);
	~Graphics();

	void renderScreen(SDL_Texture* title, Settings* s);
	void reset(Settings* s);

	SDL_Rect* getMenuRects();
	SDL_Texture** getBoxSprites();
	SDL_Texture** getMenuSprites();

	SDL_Window* _window;
	SDL_Renderer* _renderer;

private:
	void drawBackground(SDL_Texture* title, int iconSize);
	void drawMenu(Settings* s);
	void initSprites(int iconSize, bool initRect);

	// Check 'Graphics' in notes.txt for array details.
	SDL_Texture* menuSprites[6];
	SDL_Rect menuRect[7]; // use the sixth one for popup close buttons, set position every time one opens
	SDL_Texture* boxSprites[5];

	Uint8 iconColor, bgColor;
};