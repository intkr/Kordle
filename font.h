#pragma once

#include <string>

#include "SDL.h"
#include "SDL_ttf.h"

#include "graphics.h"

class Font {
public:
	Font(Graphics* g);
	~Font();

	void reset(Graphics* g);

	SDL_Texture* getTitleTexture();
	SDL_Texture* getLetterTexture(SDL_Texture* _texture, SDL_Renderer* _renderer, short* jamo, bool isColored);
	TTF_Font* getTextFont();
	TTF_Font* getTitleFont();
	void setTextFontSize(int size);

	/* returns a pointer of a pre - made SDL_Color.
	 Parameters :
	 0 = Title text
	 1 = Game text (Default)
	 2 = Game text (Colored)
	 3 = Normal text
	 4 = Inverse colored text
	 */
	SDL_Color* getColor(int n);
private:
	TTF_Font* titleFont;
	TTF_Font* boxFont;
	TTF_Font* textFont;
	SDL_Surface* textSurface;
	SDL_Texture* titleTexture;
	SDL_Color titleColor;
	SDL_Color defaultGameColor;
	SDL_Color coloredGameColor;
	SDL_Color textColor;
	SDL_Color inverseColor;

	int getConsonantUNICODE(int ja);
};

int separateUTF8Korean(unsigned short* result, wchar_t* data);
std::wstring combineJamo(unsigned short* data);