#pragma once
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "settings.h"

class Graphics {
public:
	Graphics(Settings* s);
	~Graphics();

	// Renders the game screen within this function.
	// Other sub-functions, including external ones are called within.
	void renderScreen(SDL_Texture* title, Settings* s);

	// Returns the pointer for menuRect.
	SDL_Rect* getMenuRects();

	SDL_Window* _window;
	SDL_Renderer* _renderer;

private:
	void drawBackground(SDL_Texture* title, int iconSize);
	void drawMenu(Settings* s);
	void initSprites(int iconSize);

	// check notes.txt
	SDL_Texture* menuSprites[5];
	SDL_Rect menuRect[6]; // use the sixth one for popup close buttons, set position every time one opens
};