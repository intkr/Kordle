#include "font.h"

Font::Font(Graphics* g) {
	gameFont = TTF_OpenFont("assets/game.ttf", 38);
	textFont = TTF_OpenFont("assets/text.ttf", 38);
	WHITE.r = WHITE.g = WHITE.b = WHITE.r = 255;

	titleTexture = SDL_CreateTextureFromSurface(g->_renderer, TTF_RenderUTF8_Blended(textFont, "Kordle", WHITE));
	textTexture = SDL_CreateTextureFromSurface(g->_renderer, TTF_RenderUTF8_Blended(gameFont, 0, WHITE));
}

Font::~Font() {
	TTF_CloseFont(gameFont);
	TTF_CloseFont(textFont);
	SDL_DestroyTexture(titleTexture);
}

SDL_Texture* Font::getTitleTexture() {
	return titleTexture;
}
// wchar_t[3] -> unsigned short[3]
// Takes a 3-byte UTF-8 character and returns 3 numbers based on the link below.
// Refer to this link for return values : https://en.wikipedia.org/wiki/Korean_language_and_computers#Hangul_in_Unicode
int separateUTF8Korean(unsigned short* result, wchar_t* data) {
	unsigned short uni = 0;
	uni += (data[0] - 224) << 12;
	uni += (data[1] - 128) << 6;
	uni += (data[2] - 128);
	uni -= 44032;
	result[2] = uni % 28; // final consonant
	uni -= result[2];
	result[1] = (uni % 588) / 28; // vowel
	result[0] = (uni - result[1]) / 588; // initial consonant

	return 0;
}

// unsigned short[3] -> SDL_Texture*
// Takes 3 numbers to get the unicode value for the corresponding Korean letter,
// then returns a SDL_Texture* containing a render of said letter.
SDL_Texture* Font::getLetterTexture(SDL_Renderer* _renderer, short* jamo) {
	SDL_DestroyTexture(textTexture);
	Uint16 text[2] = { 0, };
	if (jamo[0] == -1) {
		// No character to render
		text[0] = 0;
	}
	else if (jamo[1] == -1) {
		// Only a consonant to render
		text[0] = getConsonantUNICODE(jamo[0]);
	}
	else {
		// Has a syllable to render
		text[0] = 44032 + jamo[0] * 588 + jamo[1] * 28 + jamo[2];
	}
	textTexture = SDL_CreateTextureFromSurface(_renderer, TTF_RenderUNICODE_Blended(gameFont, text, WHITE));
	return textTexture;
}

// Returns the Unicode value for single consonant letters.
int Font::getConsonantUNICODE(int ja) {
	// optimize this later
	switch (ja) {
	case 0:
		return 12593;
	case 1:
		return 12594;
	case 2:
		return 12596;
	case 3:
		return 12599;
	case 4:
		return 12600;
	case 5:
		return 12601;
	case 6:
		return 12609;
	case 7:
		return 12610;
	case 8:
		return 12611;
	case 9:
		return 12613;
	case 10:
		return 12614;
	case 11:
		return 12615;
	case 12:
		return 12616;
	case 13:
		return 12617;
	case 14:
		return 12618;
	case 15:
		return 12619;
	case 16:
		return 12620;
	case 17:
		return 12621;
	case 18:
		return 12622;
	}
	return 0;
}