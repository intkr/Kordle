#pragma once

#include <stdio.h>
#include <string>

#include "SDL.h"
#include "SDL_ttf.h"

#include "graphics.h"

class Font {
public:
	Font(Graphics* g);
	~Font();

	// Returns titleTexture.
	SDL_Texture* getTitleTexture();
	SDL_Texture* getLetterTexture(SDL_Renderer* _renderer, short* jamo);
private:
	TTF_Font* gameFont;
	SDL_Texture* titleTexture;
	SDL_Texture* textTexture;
	SDL_Color WHITE;

	int getConsonantUNICODE(int ja);
};

int separateUTF8Korean(unsigned short* result, wchar_t* data);