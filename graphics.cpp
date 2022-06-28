#include "graphics.h"

/*
	boxes start at 35, 166
	each box is 62x62
	gaps are 5 pixels wide

	Blank box / Wrong letter : 58, 58, 60 (edge of blank box has a thickness of 2 pixels)
	Wrong spot : 174, 159, 77
	Correct spot : 101, 139, 85
*/

Graphics::Graphics(Settings* s) {
	SDL_CreateWindowAndRenderer(s->scrWidth, s->scrHeight, SDL_WINDOW_BORDERLESS, &_window, &_renderer);
}

Graphics::~Graphics() {
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
}

void Graphics::renderScreen(SDL_Texture* title) {
	drawBackground(title);
}

void Graphics::drawBackground(SDL_Texture* title) {
	SDL_SetRenderDrawColor(_renderer, 18, 18, 19, 255);
	SDL_RenderFillRect(_renderer, NULL);
	SDL_SetRenderDrawColor(_renderer, 58, 58, 60, 255);
	SDL_RenderDrawLine(_renderer, 0, 50, 400, 50);

	int titleWidth, titleHeight;
	SDL_QueryTexture(title, NULL, NULL, &titleWidth, &titleHeight);

	// 146, 11 ~ 254, 38
	SDL_Rect titleRect;
	titleRect.x = 141;
	titleRect.y = 7;
	titleRect.w = titleWidth;
	titleRect.h = titleHeight;
	SDL_RenderCopy(_renderer, title, NULL, &titleRect);
}