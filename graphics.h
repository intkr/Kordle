#pragma once
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "settings.h"

class Graphics {
public:
	Graphics(Settings* s);
	~Graphics();

	void renderScreen(SDL_Texture* title);

	SDL_Window* _window;
	SDL_Renderer* _renderer;

private:
	void drawBackground(SDL_Texture* title);
};