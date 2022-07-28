#pragma once

#include <string>

#include "SDL.h"
#include "SDL_ttf.h"

#include "graphics.h"

class Font {
public:
	Font(Graphics* g);
	~Font();

	SDL_Texture* getTitleTexture();
	SDL_Texture* getLetterTexture(SDL_Texture* _texture, SDL_Renderer* _renderer, short* jamo);
private:
	TTF_Font* titleFont;
	TTF_Font* boxFont;
	TTF_Font* textFont;
	SDL_Surface* textSurface;
	SDL_Texture* titleTexture;
	SDL_Color WHITE;

	int getConsonantUNICODE(int ja);
};

int separateUTF8Korean(unsigned short* result, wchar_t* data);
std::wstring combineJamo(unsigned short* data);