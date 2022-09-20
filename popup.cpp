#include "popup.h"

extern char lang;
extern bool isDarkMode;

Popup::Popup(Graphics* g, Settings* s) {
	bigPopTexture = smallPopTexture = tmpTexture = NULL;
	tmpSurface = NULL;
	smallPopFrames = openedBigPop = 0;
	popAniFrames = -1;
	tmpRect.x = tmpRect.y = tmpRect.w = tmpRect.h = 0;
	// tmpRect should be initialized everytime you use it

	_controls = new Controls(g, s->debugMode);

	reset();
	_controls->reset(g, s->debugMode);
}

Popup::~Popup() {
	// everything should be already NULL but just in case
	SDL_DestroyTexture(bigPopTexture);
	SDL_DestroyTexture(smallPopTexture);
	SDL_DestroyTexture(tmpTexture);
	SDL_FreeSurface(tmpSurface);
}

void Popup::reset() {
	if (isDarkMode) {
		smallPopColor = 255;
		bigPopColor = 27;
	}
	else {
		smallPopColor = 0;
		bigPopColor = 240;
	}
}

void Popup::reset2(Graphics* g, Font* f, Kordle* k) {
	if (openedBigPop)
		drawPopup(g, f, k, openedBigPop);
}

// only used for rendering big texts in the 'help' popup
void Popup::renderBigText(Graphics* g, Font* f, int x, int y, int unicode) {
	Uint16 text[2] = { (Uint16)unicode, 0 };
	tmpSurface = TTF_RenderUNICODE_Blended(f->getTitleFont(), text, *(f->getColor(2)));
	tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
	SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
	tmpRect.x = x + (45  - tmpRect.w) / 2;	// this probably should be fixed but im not sure rn
	tmpRect.y = y + (45  - tmpRect.h) / 2;
	SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
	SDL_DestroyTexture(tmpTexture);
}

void Popup::renderUNICODE(Graphics* g, Font* f, int x, int y, Uint16* text) {
	tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getColor(0)));
	tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
	SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
	tmpRect.x = x;
	tmpRect.y = y;
	SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
	SDL_DestroyTexture(tmpTexture);
}

void Popup::renderUTF8(Graphics* g, Font* f, int x, int y, const char* text) {
	tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text, *(f->getColor(0)));
	tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
	SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
	tmpRect.x = x;
	tmpRect.y = y;
	SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
	SDL_DestroyTexture(tmpTexture);
}

void Popup::renderPopup(Graphics* g, Font* f) {
	if (smallPopFrames > 0) {
		if (popAniFrames == -1) {
			popAniFrames = 10;
		}
		else if (popAniFrames) {
			popAniFrames--;
		}
		SDL_QueryTexture(smallPopTexture, NULL, NULL, &(tmpRect.w), &(tmpRect.h));
		tmpRect.y = 575;
		tmpRect.x = (400  - tmpRect.w) / 2;
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
	if (openedBigPop) {
		if (popAniFrames == -1) {
			popAniFrames = 10;
		}
		else if (popAniFrames) {
			popAniFrames--;
		}
		// dim game so popup stands out more
		if (popAniFrames > 0) {
			if (isDarkMode) {
				SDL_SetRenderDrawColor(g->_renderer, 0, 0, 0, (Uint8)((double)255 * (10 - popAniFrames) * 0.05));
			}
			else {
				SDL_SetRenderDrawColor(g->_renderer, 255, 255, 255, (Uint8)((double)255 * (10 - popAniFrames) * 0.05));
			}
		}
		else {
			if (isDarkMode) {
				SDL_SetRenderDrawColor(g->_renderer, 0, 0, 0, (Uint8)((double)255 * 0.5));
			}
			else {
				SDL_SetRenderDrawColor(g->_renderer, 255, 255, 255, (Uint8)((double)255 * 0.5));
			}
		}


		SDL_RenderFillRect(g->_renderer, NULL);
		SDL_QueryTexture(bigPopTexture, NULL, NULL, &(tmpRect.w), &(tmpRect.h));
		SDL_SetTextureAlphaMod(bigPopTexture, 255 * (10 - popAniFrames) / 10);
		tmpRect.y = (650  - tmpRect.h) / 2;
		tmpRect.x = (400  - tmpRect.w) / 2;
		SDL_RenderCopy(g->_renderer, bigPopTexture, NULL, &tmpRect);

		switch (openedBigPop) {
		case 202:
			_controls->drawSwitch(g, popAniFrames);
			break;
		case 200:
			drawTimer(g, f);
			break;
		}
	}
}

void Popup::drawPopup(Graphics* g, Font* f, Kordle* k, int code) {
	if (!code) return; // if no popup just return
	printf("Popup triggered (%d)\n", code);
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
		smallPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 200, 40 );
		SDL_SetRenderTarget(g->_renderer, smallPopTexture);
		SDL_SetTextureBlendMode(smallPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, smallPopColor, smallPopColor, smallPopColor, 255);
		SDL_RenderFillRect(g->_renderer, NULL);
		roundTexture(g, smallPopTexture, 10 );

		if (lang == 'k') {
			Uint16 text[] = { 49464, 32, 44544, 51088, 44032, 32, 50500, 45785, 45768, 45796, 0 };
			SDL_Surface* tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getColor(4)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		else {
			SDL_Surface* tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), "Not enough letters", *(f->getColor(4)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (200  - tmpRect.w) / 2;
		tmpRect.y = (40  - tmpRect.h) / 2;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		break;
	}
	case 1: // 없는 단어입니다
	{
		smallPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 200, 40 );
		SDL_SetRenderTarget(g->_renderer, smallPopTexture);
		SDL_SetTextureBlendMode(smallPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, smallPopColor, smallPopColor, smallPopColor, 255);
		SDL_RenderFillRect(g->_renderer, NULL);
		roundTexture(g, smallPopTexture, 10 );

		if (lang == 'k') {
			Uint16 text[] = { 50630, 45716, 32, 45800, 50612, 51077, 45768, 45796, 0 };
			SDL_Surface* tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getColor(4)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		else {
			SDL_Surface* tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), "Not in word list", *(f->getColor(4)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (200  - tmpRect.w) / 2;
		tmpRect.y = (40  - tmpRect.h) / 2;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		break;
	}
	}
	SDL_SetRenderTarget(g->_renderer, NULL);
}

void Popup::drawBigPopup(Graphics* g, Font* f, Kordle* k, int code) {
	SDL_DestroyTexture(bigPopTexture);
	switch (code) {
	case 0: // stat screen
	{
		bigPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 350, 350 );
		SDL_SetRenderTarget(g->_renderer, bigPopTexture);
		SDL_SetTextureBlendMode(bigPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, bigPopColor, bigPopColor, bigPopColor, 255);
		SDL_RenderFillRect(g->_renderer, NULL);

		g->getMenuRects()[6].y = 16;
		g->getMenuRects()[6].x = 316; // 350 - 34
		SDL_RenderCopy(g->_renderer, g->getMenuSprites()[1], NULL, &(g->getMenuRects()[6]));
		// offset menuRect[6] to compensate for popup centering
		g->getMenuRects()[6].x += 25;
		g->getMenuRects()[6].y += 150;

		f->setTextFontSize(20);
		const char *text2[6] = { "Stats\0", "Played\0", "Win %\0", "Current Streak\0", "  Max Streak\0", "Record\0"};
		char text3[] = "placeholder\0"; // used for numbers
		Uint16 text4[6][6] = { {53685, 44228, 0,}, {54540, 47112, 51060, 0}, {49849, 47456, 32, 37, 0}, {54788, 51116, 32, 50672, 49849, 0}, {52572, 45796, 32, 50672, 49849, 0}, {44592, 47197, 0} };
		if (lang == 'k') {
			tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text4[0], *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		else {
			tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text2[0], *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (350  - tmpRect.w) / 2;
		tmpRect.y = 15;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		// max 57 pixels wide, 20 pixels gap
		// 57*4 = 180 //// 20*3 = 60 //// 110
		f->setTextFontSize(25);
		for (int i = 1; i <= 4; i++) {
			_itoa_s(k->getPlayerData(i), text3, sizeof(text3), 10);
			tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text3, *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
			SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
			tmpRect.x = 45  + (57  - tmpRect.w) / 2 + 65 * (i - 1);
			tmpRect.y = 48;
			SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
			SDL_DestroyTexture(tmpTexture);
		}
		
		// center on 84 + 65n
		f->setTextFontSize(13);
		for (int i = 1; i <= 4; i++) {
			if (lang == 'k') {
				tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text4[i], *(f->getColor(3)));
				tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
				SDL_FreeSurface(tmpSurface);
			}
			else {
				tmpSurface = TTF_RenderUTF8_Blended_Wrapped(f->getTextFont(), text2[i], *(f->getColor(3)), 51 );
				tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
				SDL_FreeSurface(tmpSurface);
			}
			
			SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
			tmpRect.x = 45  + (57  - tmpRect.w) / 2 + 66 * (i - 1);
			tmpRect.y = 72;
			SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
			SDL_DestroyTexture(tmpTexture);
		}

		f->setTextFontSize(20);
		if (lang == 'k') {
			tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text4[5], *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		else {
			tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text2[5], *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (350  - tmpRect.w) / 2;
		tmpRect.y = 125;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		int correctRectNo = -1;
		int rectLength = 5;
		for (int i = 0; i < 6; i++) {
			if (k->getPlayerData(1) != 0) {
				rectLength = max((int)((float)250 * k->getPlayerData(10 + i) / k->getPlayerData(1)), 5 );
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
			tmpRect.y = (155 + i * 26);
			SDL_RenderFillRect(g->_renderer, &tmpRect);

			f->setTextFontSize(14);
			_itoa_s(k->getPlayerData(i + 10), text3, sizeof(text3), 10);
			tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), text3, *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
			tmpRect.x += tmpRect.w;
			tmpRect.y += 1;
			SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
			tmpRect.x -= (tmpRect.w + 6 );
			if (tmpRect.x <= 40  + 1) tmpRect.x += tmpRect.w + 10; // +1 for potential rounding errors
			SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
			SDL_DestroyTexture(tmpTexture);
		}
	}
	break;
	case 1: // help screen
	{
		bigPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 350, 350 );
		SDL_SetRenderTarget(g->_renderer, bigPopTexture);
		SDL_SetTextureBlendMode(bigPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, bigPopColor, bigPopColor, bigPopColor, 255);
		SDL_RenderFillRect(g->_renderer, NULL);

		g->getMenuRects()[6].y = 16;
		g->getMenuRects()[6].x = 316; // 350 - 34
		SDL_RenderCopy(g->_renderer, g->getMenuSprites()[1], NULL, &(g->getMenuRects()[6]));
		// offset menuRect[6] to compensate for popup centering
		g->getMenuRects()[6].x += 25;
		g->getMenuRects()[6].y += 150;

		if (lang == 'k') {
			Uint16 text[] = {44172, 51076, 32, 48169, 48277, 0};
			tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		else {
			tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), "How to Play", *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (350  - tmpRect.w) / 2;
		tmpRect.y = 15;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		f->setTextFontSize(14);
		
		if (lang == 'k') {
			// :skull:
			Uint16 text[] = { 50668, 49455, 32, 48264, 51032, 32, 44592, 54924, 32, 50504, 50640, 32, 45800, 50612, 47484, 32, 52286, 51004, 49464, 50836, 46, 0 };
			renderUNICODE(g, f, 20, 50, text);
			Uint16 text2[] = { 45800, 50612, 47484, 32, 52286, 51012, 32, 49688, 32, 51080, 46020, 47197, 32, 55180, 53944, 44032, 32, 51452, 50612, 51665, 45768, 45796, 46, 0 };
			renderUNICODE(g, f, 20, 70, text2);
			Uint16 text3[] = { 39, 12632, 39, 45208, 32, 39, 12602, 39, 32, 46321, 51032, 32, 44221, 50864, 44, 32, 49352, 47196, 50868, 32, 55180, 53944, 44032, 32, 51452, 50612, 51665, 45768, 45796, 46, 0 };
			renderUNICODE(g, f, 20, 160, text3);
		}
		else {
			renderUTF8(g, f, 20, 50, "Find the Kordle in 6 tries.");
			renderUTF8(g, f, 20, 70, "Hints are given to guide you find the answer.");
			renderUTF8(g, f, 20, 160, "For compound letters, special hints are given.");
		}

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
					tmpRect.y = (100 + l * 90);
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
	}
	break;
	case 2: // settings screen
	{
		bigPopTexture = SDL_CreateTexture(g->_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 350, 350 );
		SDL_SetRenderTarget(g->_renderer, bigPopTexture);
		SDL_SetTextureBlendMode(bigPopTexture, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(g->_renderer, bigPopColor, bigPopColor, bigPopColor, 255);
		SDL_RenderFillRect(g->_renderer, NULL);

		g->getMenuRects()[6].y = 16;
		g->getMenuRects()[6].x = 316; // 350 - 34
		SDL_RenderCopy(g->_renderer, g->getMenuSprites()[1], NULL, &(g->getMenuRects()[6]));
		// offset menuRect[6] to compensate for popup centering
		g->getMenuRects()[6].x += 25;
		g->getMenuRects()[6].y += 150;

		if (lang == 'k') {
			Uint16 text[] = {49444, 51221, 0};
			tmpSurface = TTF_RenderUNICODE_Blended(f->getTextFont(), text, *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		else {
			tmpSurface = TTF_RenderUTF8_Blended(f->getTextFont(), "Settings", *(f->getColor(3)));
			tmpTexture = SDL_CreateTextureFromSurface(g->_renderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
		}
		SDL_QueryTexture(tmpTexture, NULL, NULL, &tmpRect.w, &tmpRect.h);
		tmpRect.x = (350  - tmpRect.w) / 2;
		tmpRect.y = 15;
		SDL_RenderCopy(g->_renderer, tmpTexture, NULL, &tmpRect);
		SDL_DestroyTexture(tmpTexture);

		f->setTextFontSize(15);
		// 60, 105, 150
		if (lang == 'k') {
			Uint16 text[] = { 44172, 51076, 32, 49353, 49345, 0 }; // 게임 색상
			renderUNICODE(g, f, 20, 62, text);
			Uint16 text2[] = { 54868, 51060, 53944, 0 }; // 화이트
			renderUNICODE(g, f, 172, 62, text2);
			Uint16 text3[] = { 48660, 47001, 0 }; // 블랙
			renderUNICODE(g, f, 275, 62, text3);
			Uint16 text5[] = { 50616, 50612, 0 }; // 언어
			renderUNICODE(g, f, 20, 107, text5);
			Uint16 text6[] = { 46356, 48260, 44536, 32, 47784, 46300, 0 }; // 디버그 모드
			renderUNICODE(g, f, 20, 152, text6);
			Uint16 text7[] = { 50672, 46973, 0 }; // 연락
			renderUNICODE(g, f, 20, 197, text7);
		}
		else {
			renderUTF8(g, f, 20,  62, "Color Scheme");
			renderUTF8(g, f, 177, 62, "Light");
			renderUTF8(g, f, 275, 62, "Dark");

			renderUTF8(g, f, 20, 107, "Language");

			renderUTF8(g, f, 20, 152, "Debug Mode");
			renderUTF8(g, f, 20, 197, "Contacts");
		}
		Uint16 textkr[] = { 54620, 44544, 0 };
		renderUNICODE(g, f, 185, 107, textkr);
		renderUTF8(g, f, 275, 107, "Eng");

		renderUTF8(g, f, 191, 152, "Off");
		renderUTF8(g, f, 275, 152, "On");

		renderUTF8(g, f, 185, 197, "intkr@khu.ac.kr"); // email reveal omg!!!
		
	}
	break;
	}
	f->setTextFontSize(-1);
	SDL_SetRenderTarget(g->_renderer, NULL);
}

void Popup::drawTimer(Graphics* g, Font* f) {
	long kstTime = time(NULL) + 9 * 60 * 60;
	int secondsLeft = ((int)floor((double)kstTime / 86400) + 1) * 86400 - kstTime;
	char timerText[9];
	snprintf(timerText, sizeof(timerText), "%02d:%02d:%02d", secondsLeft / 3600, (secondsLeft % 3600) / 60, secondsLeft % 60);

	// why is there a memory leak??
	if (lang == 'k') {
		Uint16 krTimer[17] = { 45796, 51020, 32, 45800, 50612, 44620, 51648, 32, };
		for (int i = 0; i < sizeof(timerText) - 1; i++) {
			if (timerText[i] == ':') {
				krTimer[i + 8] = 58;
			}
			else {
				krTimer[i + 8] = atoi((const char *)timerText[i]) + 48;
			}
		}
		renderUNICODE(g, f, 25 + 20, 125 + 315, krTimer);
	}
	else {
		char engTimer[21] = { "Time until \0" };
		strcat_s(engTimer, timerText);
		renderUTF8(g, f, 25 + 20, 150 + 315, engTimer);
	}
}

void Popup::closeBigPopup() {
	openedBigPop = 0;
	SDL_DestroyTexture(bigPopTexture);
	popAniFrames = -1;
}

void Popup::updateSwitch(int n) {
	printf("Switch (%d)\n", n);
	bool* switchSide = &_controls->switches[n]._side;
	int* switchFrame = &_controls->switches[n].aniFrame;
	*switchSide = !*switchSide;
	if (*switchSide) {
		// going right
		if (*switchFrame < 0) {

		}
		else {
			*switchFrame = 1;
		}
	}
	else {
		// going left
		if (*switchFrame > 0) {

		}
		else {
			*switchFrame = -1;
		}
	}
}

// reset the textureblendmode and renderdrawcolor after usage
void Popup::roundTexture(Graphics* g, SDL_Texture* t, int base) {
	SDL_BlendMode tb, rb;
	Uint8 rgba[4];
	SDL_GetRenderDrawBlendMode(g->_renderer, &rb);
	SDL_GetTextureBlendMode(t, &tb);
	SDL_GetRenderDrawColor(g->_renderer, &rgba[0], &rgba[1], &rgba[2], &rgba[3]);
	SDL_SetRenderDrawBlendMode(g->_renderer, SDL_BLENDMODE_NONE);
	SDL_SetTextureBlendMode(t, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(g->_renderer, 18, 18, 18, 0);
	int x, y, w, h;
	SDL_QueryTexture(t, NULL, NULL, &w, &h);
	// i values :
	// 0 1
	// 2 3

	// very wack but it does the job done
	for (int i = 0; i < 4; i++) {
		x = (i % 2) ? w - base - 1 : base;
		y = (i < 2) ? 0 : h - 1;
		for (int j = 0; j < base; j++) {
			while (((x - ((i % 2) ? w - base - 1 : base)) * (x - ((i % 2) ? w - base - 1 : base)) + (y - ((i < 2) ? base : h - base - 1)) * (y - ((i < 2) ? base : h - base - 1))) <= (base * base)) {
				(i % 2) ? x++ : x--;
			}
			SDL_RenderDrawLine(g->_renderer, x, y, (i % 2) ? w : 0, y);
			(i < 2) ? y++ : y--;
		}
	}
	SDL_SetRenderDrawBlendMode(g->_renderer, rb);
	SDL_SetTextureBlendMode(t, tb);
	SDL_SetRenderDrawColor(g->_renderer, rgba[0], rgba[1], rgba[2], rgba[3]);
}