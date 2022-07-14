#include "kordle.h"

Kordle::Kordle() {
	rk = -1;
	// initialize stuff to default
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 3; k++) {
				_input[j][i].color[k] = 0;
				_input[j][i].jamo[k] = -1;
			}
			_input[j][i].jamo[2] = 0;
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
		for (int i = 0; i <= (signed int)playedGames - 1; i++) {
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

void Kordle::handleInput(int key) {
	if (tries == -1) return;
	/* Steps
	- Find the rightmost key input from two variables (_input and tries).
	  (abbr. RK)
	- define behaviors for which type of key RK is
		* check if there is room for input first before performing steps below

		- a) initial consonant
			if key is a consonant, no actions
			if key is a vowel, add the vowel to _input and return
		- b) vowel
			if key is a consonant, add the final consonant to _input and return
			if key is a vowel, check if the two vowels are combinable
				if so, edit the vowel and return
				if not, no actions
		- c) final consonant
			if key is a consonant, check if the two consonants are combinable
				if so, edit the final consonant and return
				if not, add the initial consonant to the next box input and return
			if key is a vowel, check if RK is a combined consonant
				if so, split the consonant; the first half stays where it belongs,
					   and the other half moves on to the next key + add the vowel and return
				if not, move RK to the next box and add the vowel, then return
	*/
	rk = findRK();
	if (key == 98) {
		// Backspace
		if (rk == -1) return;
		switch (rk % 3) {
		case 0:
		case 1:
			_input[tries][rk / 3].jamo[rk % 3] = -1;
			break;
		case 2:
			_input[tries][rk / 3].jamo[rk % 3] = 0;
			break;
		}
		return;
	}
	if (rk == -1) {
		_input[tries][0].jamo[0] = key - 1;
	}
	switch (rk % 3) {
	case 0:
		// initial consonant
		if (key >= 20) {
			// key : vowel
			_input[tries][rk / 3].jamo[1] = key - 20;
		}
		break;
	case 1:
		// vowel
		if (key >= 20) {
			// key : vowel
			switch (_input[tries][rk / 3].jamo[1]) {
			case 8:
				switch (key - 20) {
				case 0: // ㅘ
					_input[tries][rk / 3].jamo[1] = 9;
					break;
				case 1: // ㅙ
					_input[tries][rk / 3].jamo[1] = 10;
					break;
				case 20: // ㅚ
					_input[tries][rk / 3].jamo[1] = 11;
					break;
				}
				break;
			case 13:
				switch (key - 20) {
				case 4: // ㅝ
					_input[tries][rk / 3].jamo[1] = 14;
					break;
				case 5: // ㅞ
					_input[tries][rk / 3].jamo[1] = 15;
					break;
				case 20: // ㅟ
					_input[tries][rk / 3].jamo[1] = 16;
					break;
				}
				break;
			case 18:
				if (key == 40) // ㅢ
					_input[tries][rk / 3].jamo[1] = 19;
				break;
			}
		}
		else {
			// key : consonant -> final
			switch (key) {
			case 1:
			case 2:
				_input[tries][rk / 3].jamo[2] = key;
				break;
			case 3:
				_input[tries][rk / 3].jamo[2] = 4;
				break;
			case 4:
				_input[tries][rk / 3].jamo[2] = 7;
				break;
			case 6:
				_input[tries][rk / 3].jamo[2] = 8;
				break;
			case 7:
			case 8:
				_input[tries][rk / 3].jamo[2] = key + 9;
				break;
			case 10:
				_input[tries][rk / 3].jamo[2] = 19;
				break;
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
				_input[tries][rk / 3].jamo[2] = key + 9;
				break;
			default:
				break;
			}
		}
		break;
	case 2:
		// final consonant
		if (key >= 20) {
			// key : vowel
			if (rk >= 9) break; // input is on last box, no room for more
			switch (_input[tries][rk / 3].jamo[2]) {
			case 1:
			case 9:
				_input[tries][rk / 3 + 1].jamo[0] = 0;
				break;
			case 2:
				_input[tries][rk / 3 + 1].jamo[0] = 1;
				break;
			case 4:
				_input[tries][rk / 3 + 1].jamo[0] = 2;
				break;
			case 7:
				_input[tries][rk / 3 + 1].jamo[0] = 3;
				break;
			case 8:
				_input[tries][rk / 3 + 1].jamo[0] = 5;
				break;
			case 10:
			case 16:
				_input[tries][rk / 3 + 1].jamo[0] = 6;
				break;
			case 11:
			case 17:
				_input[tries][rk / 3 + 1].jamo[0] = 7;
				break;
			case 3:
			case 12:
			case 18:
			case 19:
				_input[tries][rk / 3 + 1].jamo[0] = 9;
				break;
			case 20:
				_input[tries][rk / 3 + 1].jamo[0] = 10;
				break;
			case 21:
				_input[tries][rk / 3 + 1].jamo[0] = 11;
				break;
			case 5:
			case 22:
				_input[tries][rk / 3 + 1].jamo[0] = 12;
				break;
			case 23:
				_input[tries][rk / 3 + 1].jamo[0] = 14;
				break;
			case 24:
				_input[tries][rk / 3 + 1].jamo[0] = 15;
				break;
			case 13:
			case 25:
				_input[tries][rk / 3 + 1].jamo[0] = 16;
				break;
			case 14:
			case 26:
				_input[tries][rk / 3 + 1].jamo[0] = 17;
				break;
			case 6:
			case 15:
			case 27:
				_input[tries][rk / 3 + 1].jamo[0] = 18;
				break;
			default:
				// shouldn't happen
				printf_s("handleInput case 2 error : %d\n", _input[tries][rk / 3].jamo[2]);
			}
			_input[tries][rk / 3 + 1].jamo[1] = key - 20;

			switch (_input[tries][rk / 3].jamo[2]) {
			case 3: // ㄳ
				_input[tries][rk / 3].jamo[2] = 1;
				break;
			case 5: // ㄵ
			case 6: // ㄶ
				_input[tries][rk / 3].jamo[2] = 4;
				break;
			case 9: // ㄺ
			case 10: // ㄻ
			case 11: // ㄼ
			case 12: // ㄽ
			case 13: // ㄾ
			case 14: // ㄿ
			case 15: // ㅀ
				_input[tries][rk / 3].jamo[2] = 8;
				break;
			case 18: // ㅄ
				_input[tries][rk / 3].jamo[2] = 17;
				break;
			default:
				// final consonants aren't combined and can simply be moved to initial consonant
				_input[tries][rk / 3].jamo[2] = 0;
			}
		}
		else {
			// key : consonant
			bool flag = false;
			switch (_input[tries][rk / 3].jamo[2]) {
			case 1:
				if (key == 10) {
					_input[tries][rk / 3].jamo[2] = 3;
					break;
				}
				else {
					flag = true;
					break;
				}
				break;
			case 4:
				switch (key) {
				case 13:
					_input[tries][rk / 3].jamo[2] = 5;
					break;
				case 19:
					_input[tries][rk / 3].jamo[2] = 6;
					break;
				default:
					flag = true;
					break;
				}
				break;
			case 8:
				switch (key) {
				case 1:
					_input[tries][rk / 3].jamo[2] = 9;
					break;
				case 7:
					_input[tries][rk / 3].jamo[2] = 10;
					break;
				case 8:
					_input[tries][rk / 3].jamo[2] = 11;
					break;
				case 10:
					_input[tries][rk / 3].jamo[2] = 12;
					break;
				case 17:
					_input[tries][rk / 3].jamo[2] = 13;
					break;
				case 18:
					_input[tries][rk / 3].jamo[2] = 14;
					break;
				case 19:
					_input[tries][rk / 3].jamo[2] = 15;
					break;
				default:
					flag = true;
					break;
				}
				break;
			case 17:
				switch (key) {
				case 10:
					_input[tries][rk / 3].jamo[2] = 18;
					break;
				default:
					flag = true;
					break;
				}
				break;
			default:
				flag = true;
				break;
			}
			if (flag) {
				// consonants aren't combinable
				if (rk >= 9) break;
				_input[tries][rk / 3 + 1].jamo[0] = key - 1;
			}
		}
		break;
	default:
		// shouldn't happen
		break;
	}
}

int Kordle::findRK() {
	for (int i = 3; i >= 0; i--) {
		for (int j = 2; j >= 0; j--) {
			if (_input[tries][i].jamo[j] != (-1 + (j == 2))) {
				return i * 3 + j;
			}
		}
	}
	return -1; // no charcters in input
}

void Kordle::renderGame(Font* f, Graphics* g) {
	SDL_Rect boxRect;
	short data[3];
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 3; k++) {
				data[k] = _input[i][j].jamo[k];
			}
			boxRect.x = 65 + j * 70;
			boxRect.y = 100 + i * 70;

			// currently only uses color[0], fix this to split into 3 rectangles
			renderBox(g->_renderer, &boxRect, _input[i][j].color[0], g->getBoxSprites());
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
	dstRect->y += (60 - dstRect->h) / 2 - 4;
	SDL_RenderCopy(_renderer, textTexture, NULL, dstRect);
}