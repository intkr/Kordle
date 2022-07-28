#include "font.h"

Font::Font(Graphics* g) {
	titleFont = TTF_OpenFont("assets/HCRBatang.ttf", 38);
	boxFont = TTF_OpenFont("assets/MaruBuri.ttf", 38);
	textFont = TTF_OpenFont("assets/MaruBuri.ttf", 15);
	WHITE.r = WHITE.g = WHITE.b = WHITE.r = 255;

	if (false) {
		// kor
		Uint16 test[5] = { 44544, 51088, 50556, 44396, 0 };
		titleTexture = SDL_CreateTextureFromSurface(g->_renderer, TTF_RenderUNICODE_Blended(boxFont, test, WHITE));
	}
	else {
		// eng
		titleTexture = SDL_CreateTextureFromSurface(g->_renderer, TTF_RenderUTF8_Blended(boxFont, "Kordle", WHITE));
	}
	// textSurface is initialized when in use
}

Font::~Font() {
	SDL_DestroyTexture(titleTexture);
	TTF_CloseFont(titleFont);
	TTF_CloseFont(boxFont);
}

SDL_Texture* Font::getTitleTexture() {
	return titleTexture;
}


// Takes 3 numbers to get the unicode value for the corresponding Korean letter,
// then returns a SDL_Texture* containing a render of said letter.
// * jamo must be short[3].
SDL_Texture* Font::getLetterTexture(SDL_Texture* _texture, SDL_Renderer* _renderer, short* jamo) {
	SDL_DestroyTexture(_texture);
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
	textSurface = TTF_RenderUNICODE_Blended(titleFont, text, WHITE);
	_texture = SDL_CreateTextureFromSurface(_renderer, textSurface);
	SDL_FreeSurface(textSurface);
	return _texture;
}

// Returns the Unicode value for single consonant letters.
int Font::getConsonantUNICODE(int ja) {
	switch (ja) {
	case 0:
	case 1:
		return 12593 + ja;
	case 2:
		return 12596;
	case 3:
	case 4:
	case 5:
		return 12596 + ja;
	case 6:
	case 7:
	case 8:
		return 12603 + ja;
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
		return 12604 + ja;
	}
	return 0;
}

// Takes a 3-byte UTF-8 character and returns 3 numbers based on the link below.
// Refer to this link for return values : https://en.wikipedia.org/wiki/Korean_language_and_computers#Hangul_in_Unicode
// * result and dat must be unsigned short[3] and wchar_t[3] respectively.
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

// Reverse function of separateUTF8Korean.
// Takes 9 jamo numbers from data,
// and returns the original text in std::wstring form.
std::wstring combineJamo(unsigned short* data) {
	unsigned short uni;
	wchar_t cline[10] = { 0, };
	for (int i = 0; i < 3; i++) {
		uni = 44032 + data[i * 3 + 0] * 588 + data[i * 3 + 1] * 28 + data[i * 3 + 2];
		for (int j = 0; j < 3; j++) {
			cline[i * 3 + 0] = (uni >> 12) + 224;
			cline[i * 3 + 1] = ((uni >> 6) & 63) + 128;
			cline[i * 3 + 2] = (uni & 63) + 128;
		}
	}
	return std::wstring(cline);
}