#include "popup.h"

extern char lang;

Popup::Popup() {
	bigPopTexture = smallPopTexture = tmpTexture = NULL;
	smallPopFrames = isBigPopOpen = 0;
	popAniFrames = -1;
	// tmpRect should be initialized everytime you use it
}

Popup::~Popup() {

}

// only used for rendering big texts in the 'help' popup
void Popup::renderBigText(Graphics* g, Font* f, int x, int y, int unicode) {
	Uint16 text[2] = { unicode, 0 };
	tmpSurface = TTF_RenderUNICODE_Blended(f->getTitleFont(), text, *(f->getColor(0)));
	tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
	SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
	tmpRect.x = x + (45 - tmpRect.w) / 2;
	tmpRect.y = y + (45 - tmpRect.h) / 2;
	SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
	SDL_DestroyTexture(tmpTexture);
}

void Popup::renderPopup(Graphics* g) {
	if (smallPopFrames > 0) {
		if (popAniFrames == -1) {
			popAniFrames = 10;
		}
		else if (popAniFrames) {
			popAniFrames--;
		}
		SDL_QueryTexture(smallPopTexture, NULL, NULL, &(tmpRect.w), &(tmpRect.h));
		tmpRect.y = 575;
		tmpRect.x = (400 - tmpRect.w) / 2;
		if (popAniFrames) {
			SDL_SetTextureAlphaMod(smallPopTexture, 255 * (10 - popAniFrames) / 10);
		}
		else {
			SDL_SetTextureAlphaMod(smallPopTexture, min(255, smallPopFrames * 8));
		}
		SDL_RenderCopy(g->_renderer, smallPopTexture, NULL, &tmpRect);
		if (--smallPopFrames == 0) {
			popAniFrames = -1;
			SDL_DestroyTexture(smallPopTexture);
		}
	}
	if (isBigPopOpen) {
		if (popAniFrames == -1) {
			popAniFrames = 10;
		}
		else if (popAniFrames) {
			popAniFrames--;
		}
		SDL_SetRenderDrawColor(g->_renderer, 0, 0, 0, 128);
		SDL_RenderFillRect(g->_renderer, NULL);
		SDL_QueryTexture(bigPopTexture, NULL, NULL, &(tmpRect.w), &(tmpRect.h));
		SDL_SetTextureAlphaMod(bigPopTexture, 255 * (10 - popAniFrames) / 10);
		tmpRect.y = (650 - tmpRect.h) / 2;
		tmpRect.x = (400 - tmpRect.w) / 2;
		SDL_RenderCopy(g->_renderer, bigPopTexture, NULL, &tmpRect);
	}
}

void Popup::drawPopup(Graphics* g, Font* f, Kordle* k, int code) {
	if (!code) return; // if no popup just return
	if (code < 200) {
		// small popup
		drawSmallPopup(g, f, code - 100);
	}
	else {
		// big popup
		drawBigPopup(g, f, k, code - 200);
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
		roundSmallPopup(g, smallPopTexture);

		Uint16 text[] = { 49464, 32, 44544, 51088, 44032, 32, 50500, 45785, 45768, 45796, 0 };
		SDL_Surface* tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getColor(0)));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
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
		roundSmallPopup(g, smallPopTexture);

		Uint16 text[] = { 50630, 45716, 32, 45800, 50612, 51077, 45768, 45796, 0 };
		SDL_Surface* tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getColor(0)));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (200 - tmpRect.w) / 2;
		tmpRect.y = (40 - tmpRect.h) / 2;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		break;
	}
	}
	SDL_SetRenderTarget(g->_renderer, NULL);
}

void Popup::drawBigPopup(Graphics* g, Font* f, Kordle* k, int code) {
	switch (code) {
	case 0: // stat screen
	{
		bigPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 350, 350);
		SDL_SetRenderTarget(g->_renderer, bigPopTexture);
		SDL_SetTextureBlendMode(bigPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, 18, 18, 19, 255);
		SDL_RenderFillRect(g->_renderer, NULL);

		g->getMenuRects()[6].y = 16;
		g->getMenuRects()[6].x = 316; // 350 - 34
		SDL_RenderCopy(g->_renderer, g->getMenuSprites()[1], NULL, &(g->getMenuRects()[6]));
		// offset menuRect[6] to compensate for popup centering
		g->getMenuRects()[6].x += 25;
		g->getMenuRects()[6].y += 150;

		Uint16 text[] = { 0 }; // use this for korean later idk
		f->setTextFontSize(20);
		const char *text2[6] = { "STATISTICS\0", "Played\0", "Win %\0", "Current Streak\0", "  Max Streak\0", "GUESS DISTRIBUTION\0"};
		char text3[] = "placeholder\0";
		tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text2[0], *(f->getColor(0)));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (350 - tmpRect.w) / 2;
		tmpRect.y = 15;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		// max 57 pixels wide, 20 pixels gap
		// 57*4 = 180 //// 20*3 = 60 //// 110
		f->setTextFontSize(25);
		for (int i = 1; i <= 4; i++) {
			_itoa_s(k->getPlayerData(i), text3, sizeof(text3), 10);
			tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text3, *(f->getColor(0)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
			SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
			tmpRect.x = 45 + (57 - tmpRect.w) / 2 + 65 * (i - 1);
			tmpRect.y = 48;
			SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
			SDL_DestroyTexture(tmpTexture);
		}
		
		// center on 84 + 65n
		f->setTextFontSize(12);
		for (int i = 1; i <= 4; i++) {
			tmpSurface = TTF_RenderUTF8_Blended_Wrapped(f->getTextFont(), text2[i], *(f->getColor(0)), 45);
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
			SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
			tmpRect.x = 45 + (57 - tmpRect.w) / 2 + 66 * (i - 1);
			tmpRect.y = 72;
			SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
			SDL_DestroyTexture(tmpTexture);
		}

		f->setTextFontSize(20);
		tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text2[5], *(f->getColor(0)));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (350 - tmpRect.w) / 2;
		tmpRect.y = 125;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		int correctRectNo = -1;
		int rectLength = 5;
		for (int i = 0; i < 6; i++) {
			if (k->getPlayerData(1) != 0) {
				rectLength = max((int)((float)250 * k->getPlayerData(10 + i) / k->getPlayerData(1)), 5);
			}

			// if k->isPlayable, all rects should be gray (58, 58, 60)
			// else if k->foundAnswer, (tries)th rect should be green (97, 140, 85) while others gray
			// else, all rects should be gray
			if (!k->isTypable && k->foundAnswer)
				correctRectNo = k->getPlayerData(0);
			if (correctRectNo == i + 1) {
				SDL_SetRenderDrawColor(g->_renderer, 97, 140, 85, 255);
			}
			else {
				SDL_SetRenderDrawColor(g->_renderer, 58, 58, 60, 255);
			}

			tmpRect.h = 18;
			tmpRect.w = rectLength;
			tmpRect.x = 50;
			tmpRect.y = 155 + i * 26;
			SDL_RenderFillRect(g->_renderer, &tmpRect);

			f->setTextFontSize(14);
			_itoa_s(k->getPlayerData(i + 10), text3, sizeof(text3), 10);
			tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text3, *(f->getColor(0)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
			tmpRect.x += tmpRect.w;
			tmpRect.y += 1;
			SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
			tmpRect.x -= tmpRect.w + 6;
			if (tmpRect.x == 40) tmpRect.x += tmpRect.w + 10;
			SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
			SDL_DestroyTexture(tmpTexture);
		}
		break;
	}
	case 1: // help screen
	{
		bigPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 350, 350);
		SDL_SetRenderTarget(g->_renderer, bigPopTexture);
		SDL_SetTextureBlendMode(bigPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, 18, 18, 19, 255);
		SDL_RenderFillRect(g->_renderer, NULL);

		g->getMenuRects()[6].y = 16;
		g->getMenuRects()[6].x = 316; // 350 - 34
		SDL_RenderCopy(g->_renderer, g->getMenuSprites()[1], NULL, &(g->getMenuRects()[6]));
		// offset menuRect[6] to compensate for popup centering
		g->getMenuRects()[6].x += 25;
		g->getMenuRects()[6].y += 150;

		f->setTextFontSize(14);
		
		// too lazy rn, improve this later
		Uint16 text[] = { 50668, 49455, 32, 48264, 51032, 32, 44592, 54924, 32, 50504, 50640, 32, 45800, 50612, 47484, 32, 52286, 51004, 49464, 50836, 46, 0, };
		SDL_Surface* tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getColor(0)));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = 20;
		tmpRect.y = 50;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		Uint16 text2[] = {45800, 50612, 47484, 32, 52286, 51012, 32, 49688, 32, 51080, 46020, 47197, 32, 55180, 53944, 44032, 32, 51452, 50612, 51665, 45768, 45796, 46, 0,};
		tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text2, *(f->getColor(0)));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = 20;
		tmpRect.y = 70;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		Uint16 text3[] = {39, 12632, 39, 45208, 32, 39, 12602, 39, 32, 46321, 51032, 32, 44221, 50864, 44, 32, 49352, 47196, 50868, 32, 55180, 53944, 44032, 32, 51452, 50612, 51665, 45768, 45796, 46, 0,};
		tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text3, *(f->getColor(0)));
		tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = 20;
		tmpRect.y = 160;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		// boxes
		tmpRect.w = 45;
		tmpRect.h = 15;
		tmpRect.y = 100;
		int boxColors[] = { 2, 4, 1, 2, 1, 4, 1, 3, 4, 1, 3, 4, 1, 4, 3 };
		for (int l = 0; l < 2; l++) {
			for (int i = 0; i < 2; i++) {
				for (int j = 0; j < 2; j++) {
					for (int k = 0; k < 3; k++) {
						SDL_RenderCopy(g->_renderer, g->getBoxSprites()[i ? 4 : boxColors[l * 6 + j * 3 + k]], NULL, &tmpRect);
						tmpRect.y += 15;
					}
					tmpRect.y = 100 + l * 90;
					tmpRect.x += 50;
				}
				tmpRect.x += 20;
			}
			tmpRect.y += 90;
			tmpRect.x = 20;
		}

		for (int i = 0; i < 2; i++) {
			tmpRect.y = 260;
			for (int j = 0; j < 3; j++) {
				SDL_RenderCopy(g->_renderer, g->getBoxSprites()[i ? 4 : boxColors[12 + j]], NULL, &tmpRect);
				tmpRect.y += 15;
			}
			tmpRect.x += 70;
		}

		// big letter

		renderBigText(g, f, 20, 100, 50672); // 연
		renderBigText(g, f, 70, 100, 44256); // 고

		renderBigText(g, f, 140, 100, 44221); // 경
		renderBigText(g, f, 190, 100, 55148); // 희

		renderBigText(g, f, 20, 190, 48148); // 바
		renderBigText(g, f, 70, 190, 48372); // 보

		renderBigText(g, f, 140, 190, 49324); // 사
		renderBigText(g, f, 190, 190, 44284); // 과

		renderBigText(g, f, 20, 260, 47568); // 말

		renderBigText(g, f, 90, 260, 45805); // 닭
		break;
	}
	case 2: // settings screen
	{
		bigPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 350, 350);
		SDL_SetRenderTarget(g->_renderer, bigPopTexture);
		SDL_SetTextureBlendMode(bigPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, 18, 18, 19, 255);
		SDL_RenderFillRect(g->_renderer, NULL);

		g->getMenuRects()[6].y = 16;
		g->getMenuRects()[6].x = 316; // 350 - 34
		SDL_RenderCopy(g->_renderer, g->getMenuSprites()[1], NULL, &(g->getMenuRects()[6]));
		// offset menuRect[6] to compensate for popup centering
		g->getMenuRects()[6].x += 25;
		g->getMenuRects()[6].y += 150;
	}
	}
	f->setTextFontSize(-1);
	SDL_SetRenderTarget(g->_renderer, NULL);
}

void Popup::closeBigPopup() {
	isBigPopOpen = false;
	SDL_DestroyTexture(bigPopTexture);
	popAniFrames = -1;
}

// reset the textureblendmode and renderdrawcolor after usage
void Popup::roundSmallPopup(Graphics* g, SDL_Texture* t) {
	SDL_BlendMode tb, rb;
	Uint8 rgba[4];
	SDL_GetRenderDrawBlendMode(g->_renderer, &rb);
	SDL_GetTextureBlendMode(t, &tb);
	SDL_GetRenderDrawColor(g->_renderer, &rgba[0], &rgba[1], &rgba[2], &rgba[3]);
	SDL_SetRenderDrawBlendMode(g->_renderer, SDL_BLENDMODE_NONE);
	SDL_SetTextureBlendMode(t, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(g->_renderer, 0, 0, 0, 255);
	int base = 10;
	int x, y, w, h;
	SDL_QueryTexture(t, NULL, NULL, &w, &h);
	// i values :
	// 0 1
	// 2 3

	// very wack but it does the job done
	for (int i = 0; i < 4; i++) {
		x = (i % 2) ? w - base : base;
		y = (i < 2) ? 0 : h;
		for (int j = 0; j < base; j++) {
			while (((x - ((i % 2) ? w - base : base)) * (x - ((i % 2) ? w - base : base)) + (y - ((i < 2) ? base : h - base)) * (y - ((i < 2) ? base : h - base))) <= (base * base)) {
				(i % 2) ? x++ : x--;
			}
			SDL_RenderDrawLine(g->_renderer, x, y, (i % 2) ? w : 0, y);
			(i < 2) ? y++ : y--;
		}
	}
	SDL_SetRenderDrawBlendMode(g->_renderer, rb);
	SDL_GetTextureBlendMode(t, &tb);
	SDL_SetRenderDrawColor(g->_renderer, rgba[0], rgba[1], rgba[2], rgba[3]);
}