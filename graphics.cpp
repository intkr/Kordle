#include "graphics.h"
#define WHITE 235
#define GRAY 155

Graphics::Graphics(Settings* s) {
	SDL_CreateWindowAndRenderer(s->getScrWidth(), s->getScrHeight(), SDL_WINDOW_BORDERLESS, &_window, &_renderer);
	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	initSprites(s->getIconSize());
}

Graphics::~Graphics() {
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	for (int i = 0; i < 5; i++) {
		SDL_DestroyTexture(menuSprites[i]);
	}
}

// Renders the default layout of the game screen.
void Graphics::renderScreen(SDL_Texture* title, Settings* s) {
	drawBackground(title, s->getIconSize());
	if (s->isMenuOpen() || (!s->isMenuOpen() && s->menuAnimationNo != -1)) {
		drawMenu(s);
	}
}


// Draws sprites with a little bit of math and a lot of handmade anti-aliasing,
// then saves the results in menuSprites for future use.
void Graphics::initSprites(int iconSize) {

	// Allocate space
	for (int i = 0; i < 6; i++) {
		// SDL_PIXELFORMAT_RGBA8888 if below doesn't accept 0~255 idk
		menuSprites[i] = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, iconSize, iconSize);
		SDL_SetRenderTarget(_renderer, menuSprites[i]);
		
		// Initialize it as transparent texture
		SDL_SetTextureBlendMode(menuSprites[i], SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
		SDL_RenderFillRect(_renderer, NULL);
	}
	for (int i = 0; i < 4; i++) {
		boxSprites[i] = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 60, 60);
		SDL_SetRenderTarget(_renderer, boxSprites[i]);
		SDL_SetTextureBlendMode(boxSprites[i], SDL_BLENDMODE_NONE);
		SDL_SetRenderDrawColor(_renderer, 18, 18, 19, 255);
		SDL_RenderFillRect(_renderer, NULL);
	}

	SDL_Rect stuffRect;

	// Boxes
	SDL_SetRenderTarget(_renderer, boxSprites[0]);
	stuffRect.x = 1;
	stuffRect.y = 1;
	stuffRect.w = 58;
	stuffRect.h = 58;
	SDL_SetRenderDrawColor(_renderer, 58, 58, 60, 255);
	SDL_RenderFillRect(_renderer, NULL);
	SDL_SetRenderDrawColor(_renderer, 18, 18, 19, 255);
	SDL_RenderFillRect(_renderer, &stuffRect);

	SDL_SetRenderTarget(_renderer, boxSprites[1]);
	SDL_RenderFillRect(_renderer, NULL);

	SDL_SetRenderTarget(_renderer, boxSprites[2]);
	SDL_SetRenderDrawColor(_renderer, 174, 159, 77, 255);
	SDL_RenderFillRect(_renderer, NULL);

	SDL_SetRenderTarget(_renderer, boxSprites[3]);
	SDL_SetRenderDrawColor(_renderer, 101, 139, 85, 255);
	SDL_RenderFillRect(_renderer, NULL);

	// Menu icon
	SDL_SetRenderTarget(_renderer, menuSprites[0]);
	menuRect[0].x = 14;
	menuRect[0].y = 16;
	menuRect[0].w = iconSize;
	menuRect[0].h = iconSize;
	
	stuffRect.x = 0;
	stuffRect.y = 0;
	stuffRect.w = iconSize;
	stuffRect.h = iconSize / 5;
	SDL_SetRenderDrawColor(_renderer, WHITE, WHITE, WHITE, 255);
	for (int i = 0; i < 3; i++) {
		SDL_RenderFillRect(_renderer, &stuffRect);
		stuffRect.y += iconSize / 5 * 2;
	}
	
	// Minimize icon
	SDL_SetRenderTarget(_renderer, menuSprites[5]);
	menuRect[5].x = 335;
	menuRect[5].y = 16;
	menuRect[5].w = iconSize;
	menuRect[5].h = iconSize;

	stuffRect.x = 0;
	stuffRect.y = 8;
	stuffRect.w = iconSize;
	stuffRect.h = iconSize / 4;
	SDL_SetRenderDrawColor(_renderer, WHITE, WHITE, WHITE, 255);
	SDL_RenderFillRect(_renderer, &stuffRect);

	// Other icon position settings
	for (int i = 2; i < 5; i++) {
		menuRect[i] = menuRect[0];
		menuRect[i].x += (i - 2) * 30 - 88;
		menuRect[i].y *= 4;
	}

	// Hint icon
		// performed anti-aliasing manually just because
	SDL_SetRenderTarget(_renderer, menuSprites[2]);
	int pythagoras;
		// Top half of semicircle
	for (int y = 0; y < 7; y++) {
		for (int x = 0; x < iconSize; x++) {
			// center of circle is (10, 7)
			pythagoras = (y - 7) * (y - 7) + (x - 10) * (x - 10);
			if (pythagoras <= 64 && pythagoras >= 16) {
				SDL_SetRenderDrawColor(_renderer, WHITE, WHITE, WHITE, 255);
				SDL_RenderDrawPoint(_renderer, x, y);
			}
			else if (pythagoras > 64 && pythagoras <= 70 && y <= 3) {
				SDL_SetRenderDrawColor(_renderer, GRAY, GRAY, GRAY, 255);
				SDL_RenderDrawPoint(_renderer, x, y);
			}
		}
	}
	SDL_SetRenderDrawColor(_renderer, WHITE, WHITE, WHITE, 255);
		// Sloped square kinda thing and the dot
	for (int y = 7; y <= 10; y++) {
		SDL_RenderDrawLine(_renderer, 20 - y, y, 23 - y, y);
	}
	for (int y = 8; y <= 9; y++) {
		SDL_RenderDrawPoint(_renderer, 24 - y, y);
	}
	for (int y = 11; y <= 12; y++) {
		SDL_RenderDrawLine(_renderer, 9, y, 15 - y / 3, y);
	}
	SDL_RenderDrawPoint(_renderer, 12, 12);
	for (int y = 16; y < 20; y++) {
		SDL_RenderDrawLine(_renderer, 9, y, 12, y);
	}
	SDL_SetRenderDrawColor(_renderer, GRAY, GRAY, GRAY, 255);
	SDL_RenderDrawPoint(_renderer, 17, 7);
	SDL_RenderDrawPoint(_renderer, 13, 11);
	SDL_RenderDrawPoint(_renderer, 14, 10);

	// Stats icon
	// 2 4 2 4 2 4 2 X
	// 2 4 2 3 2 5 2 Y
	SDL_SetRenderDrawColor(_renderer, WHITE, WHITE, WHITE, 255);
	SDL_SetRenderTarget(_renderer, menuSprites[3]);
	for (int x = 0; x < 2; x++) {
		SDL_RenderDrawLine(_renderer, x, 6, x, 19);
	}
	for (int x = 6; x < 8; x++) {
		SDL_RenderDrawLine(_renderer, x, 0, x, 19);
	}
	for (int x = 12; x < 14; x++) {
		SDL_RenderDrawLine(_renderer, x, 0, x, 19);
	}
	for (int x = 18; x < 20; x++) {
		SDL_RenderDrawLine(_renderer, x, 11, x, 19);
	}
	for (int y = 0; y < 2; y++) {
		SDL_RenderDrawLine(_renderer, 6, y, 12, y);
	}
	for (int y = 6; y < 8; y++) {
		SDL_RenderDrawLine(_renderer, 6, y, 0, y);
	}
	for (int y = 11; y < 13; y++) {
		SDL_RenderDrawLine(_renderer, 13, y, 19, y);
	}
	for (int y = 18; y < 20; y++) {
		SDL_RenderDrawLine(_renderer, 0, y, 19, y);
	}

	// Settings icon
	SDL_Texture* stuffTexture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, iconSize, iconSize);
	SDL_SetTextureBlendMode(stuffTexture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(_renderer, NULL);
	SDL_SetRenderTarget(_renderer, stuffTexture);
	SDL_SetRenderDrawColor(_renderer, WHITE, WHITE, WHITE, 255);
	stuffRect.w = 4;
	stuffRect.h = 18;
	stuffRect.x = 8;
	stuffRect.y = 1;
	SDL_RenderFillRect(_renderer, &stuffRect);
	SDL_SetRenderTarget(_renderer, menuSprites[4]);
	for (int i = 1; i <= 3; i++) {
		SDL_RenderCopyExF(_renderer, stuffTexture, NULL, NULL, 60.0 * i, NULL, SDL_FLIP_NONE);
	}
	for (int x = 0; x < iconSize; x++) {
		for (int y = 0; y < iconSize; y++) {
			pythagoras = (int)(((double)y - 9.5) * ((double)y - 9.5)) + int(((double)x - 9.5) * ((double)x - 9.5));
			if (pythagoras <= 30 && pythagoras > 9) {
				SDL_SetRenderDrawColor(_renderer, WHITE, WHITE, WHITE, 255);
				SDL_RenderDrawPoint(_renderer, x, y);
			}
			else if (pythagoras <= 9) {
				SDL_SetRenderDrawColor(_renderer, 18, 18, 19, 255);
				SDL_RenderDrawPoint(_renderer, x, y);
			}
		}
	}
		// Manual anti-aliasing
	SDL_SetRenderDrawColor(_renderer, GRAY, GRAY, GRAY, 255);
	SDL_RenderDrawPoint(_renderer, 11, 18);
	SDL_RenderDrawPoint(_renderer, 8, 18);
	SDL_RenderDrawPoint(_renderer, 11, 1);
	SDL_RenderDrawPoint(_renderer, 8, 1);
	SDL_RenderDrawPoint(_renderer, 4, 4);
	SDL_RenderDrawPoint(_renderer, 15, 4);
	SDL_RenderDrawPoint(_renderer, 15, 15);
	SDL_RenderDrawPoint(_renderer, 4, 15);

	// Close icon
	menuRect[1].x = 366;
	menuRect[1].y = 16;
	menuRect[1].w = iconSize;
	menuRect[1].h = iconSize;
	SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(_renderer, NULL);
	SDL_SetRenderTarget(_renderer, stuffTexture);
	SDL_SetRenderDrawColor(_renderer, WHITE, WHITE, WHITE, 255);
	stuffRect.w = 4;
	stuffRect.h = 20;
	stuffRect.x = 8;
	stuffRect.y = 0;
	SDL_RenderFillRect(_renderer, &stuffRect);
	SDL_SetRenderTarget(_renderer, menuSprites[1]);
	for (int i = 1; i <= 2; i++) {
		SDL_RenderCopyExF(_renderer, stuffTexture, NULL, NULL, 90.0 * i + 45.0, NULL, SDL_FLIP_NONE);
	}

	SDL_DestroyTexture(stuffTexture);
	SDL_SetRenderTarget(_renderer, NULL);
}

void Graphics::drawBackground(SDL_Texture* title, int iconSize) {
	// Stuff
	SDL_SetRenderDrawColor(_renderer, 18, 18, 19, 255);
	SDL_RenderFillRect(_renderer, NULL);
	SDL_SetRenderDrawColor(_renderer, 58, 58, 60, 255);
	SDL_RenderDrawLine(_renderer, 0, 50, 400, 50);

	int titleWidth, titleHeight;
	SDL_QueryTexture(title, NULL, NULL, &titleWidth, &titleHeight);

	// Title logo
	SDL_Rect titleRect;
	titleRect.x = 141;
	titleRect.y = 7;
	titleRect.w = titleWidth;
	titleRect.h = titleHeight;
	SDL_RenderCopy(_renderer, title, NULL, &titleRect);

	// Menu icon
	SDL_RenderCopy(_renderer, menuSprites[0], NULL, &menuRect[0]);

	// Close icon
	SDL_RenderCopy(_renderer, menuSprites[1], NULL, &menuRect[1]);

	// Minimize icon
	SDL_RenderCopy(_renderer, menuSprites[5], NULL, &menuRect[5]);
}

void Graphics::drawMenu(Settings* s) {
	if (s->menuAnimationNo == -1) {
		// no animation
		for (int i = 2; i <= 4; i++) {
			SDL_RenderCopy(_renderer, menuSprites[i], NULL, &menuRect[i]);
		}
	}
	else if (s->menuAnimationNo == 11) {	// Last frame
		if (s->isMenuOpen()) {
			// Reset x coordinates to prevent offset
			for (int i = 2; i < 5; i++) {
				menuRect[i].x = 14 + (i - 2) * 30;
			}
			s->menuAnimationNo = -1;
		}
		else {
			for (int i = 2; i <= 4; i++) {
				SDL_RenderCopy(_renderer, menuSprites[i], NULL, &menuRect[i]);
			}
			s->menuAnimationNo--;
		}
	}
	else if (s->menuAnimationNo == 0) {		// First frame
		if (s->isMenuOpen()) {
			for (int i = 2; i <= 4; i++) {
				SDL_RenderCopy(_renderer, menuSprites[i], NULL, &menuRect[i]);
			}
			s->menuAnimationNo++;
		}
		else {
			// Reset x coordinates to prevent offset
			for (int i = 2; i < 5; i++) {
				menuRect[i].x = 14 + (i - 2) * 30 - 88;
			}
			s->menuAnimationNo = -1;
		}
	}
	else {
		// perform animations
		if (s->isMenuOpen()) {
			// Opening animation
			for (int i = 2; i <= 4; i++) {
				menuRect[i].x += 8;
				SDL_RenderCopy(_renderer, menuSprites[i], NULL, &menuRect[i]);
			}
			s->menuAnimationNo++;
		}
		else {
			// Closing animation
			for (int i = 2; i <= 4; i++) {
				menuRect[i].x -= 8;
				SDL_RenderCopy(_renderer, menuSprites[i], NULL, &menuRect[i]);
			}
			s->menuAnimationNo--;
		}

	}
}

SDL_Rect* Graphics::getMenuRects() {
	return menuRect;
}

SDL_Texture** Graphics::getBoxSprites() {
	return boxSprites;
}