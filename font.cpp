#include "font.h"

Font::Font(Graphics* g) {
	gameFont = TTF_OpenFont("MaruBuri-Bold.ttf", 20);
	WHITE.r = WHITE.g = WHITE.b = WHITE.r = 255;

	titleTexture = SDL_CreateTextureFromSurface(g->_renderer, TTF_RenderUTF8_Blended(gameFont, "Kordle", WHITE));
}

Font::~Font() {
	TTF_CloseFont(gameFont);
}

SDL_Texture* Font::getTitleTexture() {
	return titleTexture;
}