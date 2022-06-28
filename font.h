#pragma once

#include <stdio.h>

#include "SDL.h"
#include "SDL_ttf.h"

#include "graphics.h"

class Font {
public:
	Font(Graphics* g);
	~Font();
	SDL_Texture* getTitleTexture();
private:
	TTF_Font* gameFont;
	SDL_Texture* titleTexture;
	SDL_Color WHITE;
};