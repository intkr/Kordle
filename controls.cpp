#include "controls.h"

extern char lang;
extern bool isDarkMode;

Controls::Controls(Graphics* g) {
	reset(g);
}

Controls::~Controls() {
	SDL_DestroyTexture(switchBodyTexture);
	SDL_DestroyTexture(switchHeadTexture);
	for (int i = _n - 1; i >= 0; i--) {
		switches[i].~Switch();
	}
}

void Controls::reset(Graphics* g) {
	_n = 3;
	int x[] = { 220, 220, 220 };
	int y[] = { 60, 105, 150 };
	bool side[] = { true, true, false };

	if (isDarkMode) {
		bgColor = 18;
		bodyColor = 116;
	}
	else {
		bgColor = 255;
		bodyColor = 116;
	}

	drawTextures(g, x, y, side);
}

void Controls::drawTextures(Graphics* g, int* _x, int* _y, bool* side) {
	SDL_DestroyTexture(switchBodyTexture);
	SDL_DestroyTexture(switchHeadTexture);
	switchBodyTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 50, 20);
	switchHeadTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 10, 10);
	SDL_SetTextureBlendMode(switchBodyTexture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(switchHeadTexture, SDL_BLENDMODE_BLEND);

	// draw switch body
	SDL_SetRenderTarget(g->_renderer, switchBodyTexture);
	SDL_SetRenderDrawColor(g->_renderer, bodyColor, bodyColor, bodyColor, 255);
	SDL_RenderFillRect(g->_renderer, NULL);
	SDL_SetRenderDrawColor(g->_renderer, bgColor, bgColor, bgColor, 255);
	tmpRect.w = 42;
	tmpRect.h = 12;
	tmpRect.x = tmpRect.y = 4;
	SDL_RenderFillRect(g->_renderer, &tmpRect);

	SDL_QueryTexture(switchBodyTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
	int base = 6, x, y;
	for (int i = 0; i < 4; i++) {
		SDL_SetRenderDrawColor(g->_renderer, bgColor, bgColor, bgColor, 255);
		x = (i % 2) ? tmpRect.w - base : base - 1;
		y = (i < 2) ? 0 : tmpRect.h - 1;
		for (int j = 0; j < base; j++) {
			while ((pow((x - ((i % 2) ? tmpRect.w - base - 1 : base)), 2) +
				pow((y - ((i < 2) ? base : tmpRect.h - base - 1)), 2)) <= pow(base, 2)) {
				(i % 2) ? x++ : x--;
			}
			SDL_RenderDrawLine(g->_renderer, x, y, (i % 2) ? tmpRect.w : 0, y);
			(i < 2) ? y++ : y--;
		}


		SDL_SetRenderDrawColor(g->_renderer, bodyColor, bodyColor, bodyColor, 255);
		x = (i % 2) ? tmpRect.w - base - 4 : base - 1 + 4;
		y = (i < 2) ? 0 + 4 : tmpRect.h - 1 - 4;
		for (int j = 0; j < base; j++) {
			while ((pow((x - ((i % 2) ? tmpRect.w - base - 1 - 4 : base + 4)), 2) +
				pow((y - ((i < 2) ? base + 4 : tmpRect.h - base - 1 - 4)), 2)) <= pow(base, 2)) {
				(i % 2) ? x++ : x--;
			}
			SDL_RenderDrawLine(g->_renderer, x, y, (i % 2) ? tmpRect.w - 4 : 0 + 4, y);
			(i < 2) ? y++ : y--;
		}
	}

	// draw switch head with some manual anti-aliasing
	SDL_SetRenderTarget(g->_renderer, switchHeadTexture);
	SDL_QueryTexture(switchHeadTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
	SDL_SetRenderDrawColor(g->_renderer, 0, 0, 0, 0);
	SDL_RenderFillRect(g->_renderer, NULL);
	SDL_SetRenderDrawColor(g->_renderer, bodyColor, bodyColor, bodyColor, 255);
	bool wasBlack = true;
	for (y = 0; y < tmpRect.h; y++) {
		for (x = 0; x < tmpRect.w; x++) {
			if (pow(x - tmpRect.w / 2, 2) + pow(y - tmpRect.h / 2, 2) <= pow((tmpRect.h / 2 - 1), 2)) {
				// gray
				if (wasBlack && x > 0) {
					SDL_SetRenderDrawColor(g->_renderer, bodyColor - 30, bodyColor - 30, bodyColor - 30, 255);
					SDL_RenderDrawPoint(g->_renderer, x - 1, y);
					SDL_SetRenderDrawColor(g->_renderer, bodyColor, bodyColor, bodyColor, 255);
				}
				wasBlack = false;
				SDL_RenderDrawPoint(g->_renderer, x, y);
			}
			else {
				// black
				if (!wasBlack) {
					SDL_SetRenderDrawColor(g->_renderer, bodyColor - 30, bodyColor - 30, bodyColor - 30, 255);
					SDL_RenderDrawPoint(g->_renderer, x, y);
					SDL_SetRenderDrawColor(g->_renderer, bodyColor, bodyColor, bodyColor, 255);
				}
				wasBlack = true;
			}
		}
	}
	SDL_SetTextureBlendMode(switchHeadTexture, SDL_BLENDMODE_BLEND);

	SDL_SetRenderTarget(g->_renderer, NULL);

	switches = new Switch[_n];

	// draw switches
	for (int n = _n; n > 0; n--) {
		switches[n - 1]._side = side[n - 1];
		switches[n - 1].dstRect.x = _x[n - 1];
		switches[n - 1].dstRect.y = _y[n - 1];
		switches[n - 1].dstRect.w = 50;
		switches[n - 1].dstRect.h = 20;
		switches[n - 1].aniFrame = 0;
		SDL_DestroyTexture(switches[n - 1]._texture);
		switches[n - 1]._texture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 50, 20);
		SDL_SetRenderTarget(g->_renderer, switches[n - 1]._texture);
		SDL_RenderCopy(g->_renderer, switchBodyTexture, NULL, NULL);

		tmpRect.y = 5;
		SDL_QueryTexture(switchHeadTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		if (switches[n - 1]._side) {
			// right
			tmpRect.x = 29;
		}
		else {
			// left
			tmpRect.x = 10;
		}
		SDL_RenderCopy(g->_renderer, switchHeadTexture, NULL, &tmpRect);
		SDL_SetTextureBlendMode(switches[n - 1]._texture, SDL_BLENDMODE_BLEND);
	}

	SDL_SetRenderDrawColor(g->_renderer, bgColor, bgColor, bgColor, 255);
	SDL_SetRenderTarget(g->_renderer, NULL);
}

int Controls::detectSwitch(int* pos) {
	pos[0] -= 25;
	pos[1] -= 150;

	for (int i = 0; i < _n; i++) {
		if (isMouseInRect(pos, &switches[i].dstRect)) {
			return 10 + i;
		}
	}
	return -1;
}

void Controls::drawSwitch(Graphics* g) {
	for (int i = 0; i < _n; i++) {
		if (abs(switches[i].aniFrame) > 15) {
			switches[i].aniFrame = 0;
			continue;
		}
		else if (switches[i].aniFrame == 0)
			continue;

		SDL_SetRenderTarget(g->_renderer, switches[i]._texture);
		SDL_SetRenderDrawColor(g->_renderer, 18, 18, 18, 0);
		SDL_RenderClear(g->_renderer);
		SDL_RenderCopy(g->_renderer, switchBodyTexture, NULL, NULL);
		int targetX;
		// move 19 pixels in 15 (FPS / 4) frames
		if (switches[i].aniFrame > 0) {
			// going right
			targetX = 10  + (int)((float)19 / 15 * switches[i].aniFrame );
		}
		else {
			// going left
			targetX = 29  - (int)((float)19 / 15 * switches[i].aniFrame );
		}
		SDL_QueryTexture(switchHeadTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);

	}
}

// duplicate of the same function in Popup because im lazy
bool Controls::isMouseInRect(int* pos, SDL_Rect* rect) {
	return (rect->w + rect->x - pos[0] > 0) && (rect->h + rect->y - pos[1] > 0) && (rect->x - pos[0] < 0) && (rect->y - pos[1] < 0);
}

Switch::Switch() {
	// :skull:
	_side = true;
	_texture = NULL;
	aniFrame = 0;
	dstRect.x = dstRect.y = dstRect.w = dstRect.h = 0;
}

Switch::~Switch() {
	SDL_DestroyTexture(_texture);
}

SDL_Texture* Switch::getSwitchTexture() {
	return _texture;
}