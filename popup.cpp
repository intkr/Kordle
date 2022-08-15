#include "popup.h"
#define min(a, b) a > b ? b : a

Popup::Popup() {
	bigPopTexture = smallPopTexture = tmpTexture = NULL;
	smallPopFrames = bigPopFrames = 0;
}

Popup::~Popup() {

}

void Popup::renderPopup(Graphics* g) {
	if (smallPopFrames > 0) {
		SDL_QueryTexture(smallPopTexture, NULL, NULL, &(tmpRect.w), &(tmpRect.h));
		tmpRect.y = 575;
		tmpRect.x = (400 - tmpRect.w) / 2;
		SDL_SetTextureAlphaMod(smallPopTexture, min(255, smallPopFrames * 8));
		SDL_RenderCopy(g->_renderer, smallPopTexture, NULL, &tmpRect);
		if (--smallPopFrames == 0) {
			SDL_DestroyTexture(smallPopTexture);
		}
	}
	if (bigPopFrames > 0) {
		SDL_SetRenderDrawColor(g->_renderer, 0, 0, 0, 128);
		SDL_RenderFillRect(g->_renderer, NULL);
		SDL_QueryTexture(bigPopTexture, NULL, NULL, &(tmpRect.w), &(tmpRect.h));
		tmpRect.y = (650 - tmpRect.h) / 2;
		tmpRect.x = (400 - tmpRect.w) / 2;
		SDL_SetTextureAlphaMod(bigPopTexture, min(255, bigPopFrames * 8));
		SDL_RenderCopy(g->_renderer, bigPopTexture, NULL, &tmpRect);
		if (--bigPopFrames == 0) {
			SDL_DestroyTexture(bigPopTexture);
		}
	}
}

void Popup::drawPopup(Graphics* g, Font* f, int code) {
	if (code < 200) {
		// small popup
		drawSmallPopup(g, f, code - 100);
	}
	else {
		// big popup
		drawBigPopup(g, f, code - 200);
	}
}

void Popup::drawSmallPopup(Graphics* g, Font* f, int code) {
	SDL_DestroyTexture(smallPopTexture);
	switch (code) {
	case 0: // 세 글자가 아닙니다
	{
		smallPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 200, 40);
		SDL_SetRenderTarget(g->_renderer, smallPopTexture);
		SDL_SetTextureBlendMode(smallPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, 100, 100, 100, 191);
		SDL_RenderFillRect(g->_renderer, NULL);

		Uint16 text[] = { 49464, 32, 44544, 51088, 44032, 32, 50500, 45785, 45768, 45796, 0 };
		SDL_Surface* textSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getWHITE()));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, textSurface);
		SDL_FreeSurface(textSurface);
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (200 - tmpRect.w) / 2;
		tmpRect.y = (40 - tmpRect.h) / 2;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		break;
	}
	case 1: // 없는 단어입니다
	{
		smallPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 200, 40);
		SDL_SetRenderTarget(g->_renderer, smallPopTexture);
		SDL_SetTextureBlendMode(smallPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, 100, 100, 100, 191);
		SDL_RenderFillRect(g->_renderer, NULL);

		Uint16 text[] = { 50630, 45716, 32, 45800, 50612, 51077, 45768, 45796, 0 };
		SDL_Surface* textSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getWHITE()));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, textSurface);
		SDL_FreeSurface(textSurface);
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (200 - tmpRect.w) / 2;
		tmpRect.y = (40 - tmpRect.h) / 2;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		break;
	}
	}
	SDL_SetRenderTarget(g->_renderer, NULL);
}

void Popup::drawBigPopup(Graphics* g, Font* f, int code) {
	switch (code) {
	case 0: //
	{
	}
	}
	SDL_SetRenderTarget(g->_renderer, NULL);
}