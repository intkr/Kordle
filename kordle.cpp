#include "kordle.h"

Kordle::Kordle() {
	// initialize stuff to default
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 3; k++) {
				input[j][i].color[k] = 0;
				input[j][i].jamo[k] = -1;
			}
			input[j][i].jamo[2] = 0;
			answer[i][j / 2] = 0;
		}
	}
	int tmp[10] = { 0, };
	std::wifstream wreader;
	std::wstring wline;

	wreader.open("assets/data.txt", std::wifstream::in);
	if (wreader.good()) {
		for (int i = 0; i < _countof(tmp); i++) {
			std::getline(wreader, wline);
			tmp[i] = _wtoi(wline.c_str());
		}
	}
	else {
		// continue as initial tmp values if error occurs
	}
	wreader.close();

	tries = tmp[0];
	playedGames = tmp[1];
	maxStreak = tmp[2];
	currentStreak = tmp[3];
	totalWon = 0;
	int tmp2 = 0;
	for (int i = 0; i < 6; i++) {
		gamesWon[i] = tmp[i + 4];
		totalWon += gamesWon[i];
		tmp2 += gamesWon[i] * (i + 1);
	}
	avgPlacement = (float)tmp2 / totalWon;

	if (playedGames != 0) {
		winrate = (int)((float)totalWon / playedGames * 100);
	}
	else {
		winrate = 0;
	}
	wreader.open("assets/list.txt", std::wifstream::in);
	bool flag = false;
	if (wreader.good()) {
		// skip previous answers
		for (int i = 0; i <= (signed int)tries - 1; i++) {
			std::getline(wreader, wline);
		}
		// actual line containing today's answer
		std::getline(wreader, wline);

		/*
		wline has UTF-8 encoded text, ie. each Korean syllable consists of 3 unsigned shorts
		(1110xxxx, 10xxxxxx, 10xxxxxx)
		it is possible to extract the unicode value of the syllable from these numbers
		by using separateUTF8Korean (font.h)
		*/

		// Initialize answer if wline isn't blank
		// (last line of list.txt needs to be an empty line so this code is necessary)
		if (wline.size() > 0) {
			unsigned short jamo[3];
			wchar_t data[3];
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 3; j++) {
					data[j] = wline[i * 3 + j];
				}
				separateUTF8Korean(jamo, data);
				for (int j = 0; j < 3; j++) {
					answer[i][j] = jamo[j];
				}
			}
		}
		else {
			flag = true;
		}
		//const wchar_t* wcharline = wline.c_str();
	}
	else {
		flag = true;
	}

	// Flag is activated if answer can't be initialized from the text file
	if (flag) {
		// resets to '오류발생'
		answer[0][0] = 11;
		answer[0][1] = 8;
		answer[1][0] = 5;
		answer[1][1] = 17;
		answer[2][0] = 7;
		answer[2][1] = 0;
		answer[2][2] = 8;
		answer[3][0] = 9;
		answer[3][1] = 1;
		answer[3][2] = 21;
	}
	wreader.close();
}

Kordle::~Kordle() {

}

void Kordle::renderGame(Font* f, Graphics* g) {
	SDL_Rect boxRect;
	short data[3];
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 3; k++) {
				data[k] = input[i][j].jamo[k];
			}
			boxRect.x = 65 + j * 70;
			boxRect.y = 100 + i * 70;

			// currently only uses color[0], fix this to split into 3 rectangles
			renderBox(g->_renderer, &boxRect, input[i][j].color[0], g->getBoxSprites());
			renderText(f, g->_renderer, &boxRect, data);
		}
		
	}
}

void Kordle::renderBox(SDL_Renderer* _renderer, SDL_Rect* dstRect, int color, SDL_Texture** box) {
	if (false) { //  animation frame number != -1

	}
	else {
		dstRect->w = dstRect->h = 60;
		SDL_RenderCopy(_renderer, box[color], NULL, dstRect);
	}
}

// Self-explanatory.
// x and y coordinates are the topleft corner of the box the text will be in.
// This function automatically repositions the texture so that it's centered.
void Kordle::renderText(Font* f, SDL_Renderer* _renderer, SDL_Rect* dstRect, short* data) {
	SDL_Texture* textTexture = f->getLetterTexture(_renderer, data);
	if (textTexture == NULL) return;
	SDL_QueryTexture(textTexture, NULL, NULL, &(dstRect->w), &(dstRect->h));
	dstRect->x += (60 - dstRect->w) / 2;
	dstRect->y += (60 - dstRect->h) / 2;
	SDL_RenderCopy(_renderer, textTexture, NULL, dstRect);
}