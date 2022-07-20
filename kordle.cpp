#include "kordle.h"

Kordle::Kordle() {
	rk = -1;
	foundAnswer = false;
	// initialize stuff to default
	for (int i = 0; i < 3; i++) {	// why is i and j flipped
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 3; k++) {
				_input[j][i].color[k] = 0;
				_input[j][i].jamo[k] = -1;
			}
			_input[j][i].aniFrame = -1;
			_input[j][i].jamo[2] = 0;
			answer[i][j / 2] = 0;
		}
	}
	int tmp[10] = { 0, };
	std::wifstream wreader;
	std::wstring wline;

	wreader.open("assets/list.csv", std::wifstream::in);
	while (wreader.good()) {
		std::getline(wreader, wline);
		wordList.insert(wline);
	}

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
	srand((unsigned int)time(NULL) / 86400);
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
	if (tries == 5) isTypable = false;
	else isTypable = true;

	wreader.open("assets/answer.csv", std::wifstream::in);
	bool flag = false;
	if (wreader.good()) {
		// skip previous answers
		// (signed int)(rand() % 365) - 1
		for (int i = 0; i <= 0 - 1; i++) {
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
			for (int i = 0; i < 3; i++) {
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
		// resets to '오류발'
		answer[0][0] = 11;
		answer[0][1] = 8;
		answer[1][0] = 5;
		answer[1][1] = 17;
		answer[2][0] = 7;
		answer[2][1] = 0;
		answer[2][2] = 8;
	}
	wreader.close();
	setAnswerData();
}

Kordle::~Kordle() {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			SDL_DestroyTexture(textTexture[i][j]);
		}
	}
}

// Returns a number corresponding to a character.
//
// * Adds an extra 50 for vowels
// * Adds an extra 100 if a final consonant isn't valid for initial ones (ex. ㄳ)
short Kordle::getJamoData(short data, int type) {
	if (type == 1) return data + 50;
	if (type == 0) return data;
	switch (data) {
	case 1:
	case 2:
		return data - 1;
	case 4:
	case 7:
		return data / 2;
	case 8:
		return 5;
	case 16:
	case 17:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
		return data - 10 + ((data >= 23) ? 1 : 0);
	default:
		return data + 100;
	}
}

// Saves jamo data of today's answer in answerData.
// Data is obtained through getJamoData().
void Kordle::setAnswerData() {
	short tmp;
	answerData.clear();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			tmp = getJamoData(answer[i][j], j);
			if (answerData.count(tmp)) {
				answerData[tmp] += 1;
			}
			else {
				answerData[tmp] = 1;
			}
		}
	}
}

// Converts jamo data of an input into wstring,
// then checks if the string is in wordList.
bool Kordle::validateAnswer() {
	unsigned short charline[9];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			charline[i * 3 + j] = _input[tries][i].jamo[j];
		}
	}
	std::wstring wline = combineJamo(charline);
	if (wordList.find(wline) != wordList.end()) {
		return true;
	}
	return false;
}

// Custom keyboard input system.
// Return value decides the input texture update procedure;
//  0 : No updates
//  1 : Forward direction update (character inputs)
// -1 : Backward direction update (backspace input)
int Kordle::handleInput(int key) {
	if (!isTypable) return false;
	/* Steps
	- Find the rightmost key input from findRK().
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
		if (rk == -1) return 0;
		switch (rk % 3) {
		case 0:
			_input[tries][rk / 3].jamo[rk % 3] = -1;
			break;
		case 1:
			switch (_input[tries][rk / 3].jamo[rk % 3]) {
			case 9:
			case 10:
			case 11:
				_input[tries][rk / 3].jamo[rk % 3] = 8;
				break;
			case 14:
			case 15:
			case 16:
				_input[tries][rk / 3].jamo[rk % 3] = 13;
				break;
			case 19:
				_input[tries][rk / 3].jamo[rk % 3] = 18;
				break;
			default:
				_input[tries][rk / 3].jamo[rk % 3] = -1;
				break;
			}
			break;
		case 2:
			switch (_input[tries][rk / 3].jamo[rk % 3]) {
			case 3:
				_input[tries][rk / 3].jamo[rk % 3] = 1;
				break;
			case 5:
			case 6:
				_input[tries][rk / 3].jamo[rk % 3] = 4;
				break;
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				_input[tries][rk / 3].jamo[rk % 3] = 8;
				break;
			case 18:
				_input[tries][rk / 3].jamo[rk % 3] = 17;
				break;
			default:
				_input[tries][rk / 3].jamo[rk % 3] = 0;
				break;
			}
			break;
		}
		return -1;
	}
	if (key == 99) {
		if (_input[tries][2].jamo[1] == -1) {
			printf("Not a word\n");
			return 0;
		}
		if (validateAnswer()) {
			checkAnswer();
			_input[tries][0].aniFrame = 1;
			tries += 1;
			isTypable = false;
		}
		else {
			printf("Not a word\n");
		}
		return 0;
	}
	if (rk == -1) {
		if (key < 20) {
			_input[tries][0].jamo[0] = key - 1;
			return 1;
		}
	}
	else if (rk != -1) {
		switch (rk % 3) {
		case 0:
			// initial consonant
			if (key >= 20) {
				// key : vowel
				_input[tries][rk / 3].jamo[1] = key - 20;
				return 1;
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
					case 1: // ㅙ
						_input[tries][rk / 3].jamo[1] = key - 11;
						break;
					case 20: // ㅚ
						_input[tries][rk / 3].jamo[1] = 11;
						break;
					default:
						return 0;
					}
					return 1;
				case 13:
					switch (key - 20) {
					case 4: // ㅝ
					case 5: // ㅞ
						_input[tries][rk / 3].jamo[1] = key - 10;
						break;
					case 20: // ㅟ
						_input[tries][rk / 3].jamo[1] = 16;
						break;
					default:
						return 0;
					}
					return 1;
				case 18:
					if (key == 40) {// ㅢ
						_input[tries][rk / 3].jamo[1] = 19;
						return 1;
					}
					return 0;
				default:
					return 0;
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
				case 10:
				case 11:
				case 12:
				case 13:
					_input[tries][rk / 3].jamo[2] = key + 9;
					break;
				case 15:
				case 16:
				case 17:
				case 18:
				case 19:
					_input[tries][rk / 3].jamo[2] = key + 8;
					break;
				case 5:
				case 9:
				case 14:
					if (rk >= 6) return 0;
					_input[tries][rk / 3 + 1].jamo[0] = key - 1;
					break;
				default:
					return 0;
					break;
				}
				return 1;
			}
			break;
		case 2:
			// final consonant
			if (key >= 20) {
				// key : vowel
				if (rk >= 6) break; // input is on last box, no room for more
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
					return 0;
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
				return 1;
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
				case 8:
					_input[tries][rk / 3].jamo[2] = key + 3;
					break;
				case 10:
					_input[tries][rk / 3].jamo[2] = 12;
					break;
				case 17:
				case 18:
				case 19:
					_input[tries][rk / 3].jamo[2] = key - 4;
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
				if (rk >= 6) break;
				_input[tries][rk / 3 + 1].jamo[0] = key - 1;
			}
			return 1;
			}
		default:
			// shouldn't happen
			break;
		}
	}
	return 0;
}

void Kordle::checkAnswer() {
	short tmp;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			tmpColor[i][j] = 0;
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			tmp = getJamoData(_input[tries][i].jamo[j], j);

			if (answerData.count(tmp)) {
				if (_input[tries][i].jamo[j] == answer[i][j]) {
					// Green
					tmpColor[i][j] = 4;
					answerData[tmp] -= 1;
				}
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (tmpColor[i][j] == 4) continue;
			tmp = getJamoData(_input[tries][i].jamo[j], j);

			if (answerData[tmp] > 0) {
				if (_input[tries][i].jamo[j] != answer[i][j]) {
					// Yellow
					tmpColor[i][j] = 2;
					answerData[tmp] -= 1;
				}
				else {
					// Green; shouldn't happen
				}
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (tmpColor[i][j] == 4 || tmpColor[i][j] == 2) continue;
			if (checkCombinedJamo(i, j)) {
				// Olive
				if (answerData[tmp] > 0) {
					tmpColor[i][j] = 3;
				}
				else {
					tmpColor[i][j] = 1;
				}
			}
			else {
				// Gray
				tmpColor[i][j] = 1;
			}
		}
	}
}

void Kordle::checkAnswerFound() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (tmpColor[i][j] != 4) {
				return;
			}
		}
	}
	foundAnswer = true;
}
bool Kordle::checkCombinedJamo(int i, int j) {
	short tmp = _input[tries][i].jamo[j];
	switch (j) {
	case 0:
		return false;
	case 1:
		// vowel
		switch (answer[i][j]) {
		case 9:
			if (tmp == 0 || tmp == 8)
				return true;
			else
				return false;
		case 10:
			if (tmp == 1 || tmp == 8)
				return true;
			else
				return false;
		case 11:
			if (tmp == 20 || tmp == 8)
				return true;
			else
				return false;
		case 14:
			if (tmp == 4 || tmp == 13)
				return true;
			else
				return false;
		case 15:
			if (tmp == 5 || tmp == 13)
				return true;
			else
				return false;
		case 16:
			if (tmp == 20 || tmp == 13)
				return true;
			else
				return false;
		case 19:
			if (tmp == 20 || tmp == 18)
				return true;
			else
				return false;
		default:
			return false;
		}
		return false;
	case 2:
		// final consonant
		switch (answer[i][j]) {
		case 3:
			if (tmp == 1 || tmp == 19)
				return true;
			else
				return false;
		case 5:
			if (tmp == 4 || tmp == 22)
				return true;
			else
				return false;
		case 9:
			if (tmp == 1 || tmp == 8)
				return true;
			else
				return false;
		case 10:
			if (tmp == 16 || tmp == 8)
				return true;
			else
				return false;
		case 11:
			if (tmp == 17 || tmp == 8)
				return true;
			else
				return false;
		case 12:
			if (tmp == 19 || tmp == 8)
				return true;
			else
				return false;
		case 13:
			if (tmp == 25 || tmp == 8)
				return true;
			else
				return false;
		case 14:
			if (tmp == 26 || tmp == 8)
				return true;
			else
				return false;
		case 15:
			if (tmp == 27 || tmp == 8)
				return true;
			else
				return false;
		case 18:
			if (tmp == 17 || tmp == 19)
				return true;
			else
				return false;
		default:
			return false;
		}
	default:
		// shouldn't happen
		return false;
	}
}

// returns the position of the last jamo character stored in _input.
// If the last jamo is in _input[0][2].jamo[1], return value is 2 * 3 + 1 = 7.
int Kordle::findRK() {
	for (int i = 2; i >= 0; i--) {
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
	short data2[2];
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 3; j++) {
			boxRect.x = 100 + j * 70;
			data[0] = i;
			data[1] = j;
			for (int k = 0; k < 3; k++) {
				boxRect.y = 100 + i * 70;
				data[2] = k;
				renderBox(g->_renderer, &boxRect, g->getBoxSprites(), data);
			}

			boxRect.y = 100 + i * 70;
			for (int k = 0; k < 3; k++) {
				data[k] = _input[i][j].jamo[k];
			}
			data2[0] = i, data2[1] = j;
			renderText(textTexture[i][j], g->_renderer, &boxRect, data, data2);
		}
		
	}
}

short Kordle::renderBox(SDL_Renderer* _renderer, SDL_Rect* dstRect, SDL_Texture** box, short* data) {
	int i = data[0], j = data[1], k = data[2];
	dstRect->w = dstRect->h = 60;
	short currentFrame = _input[i][j].aniFrame;

	if (currentFrame != -1) {
		if (foundAnswer) {
			if (currentFrame < boxWaitFrames) {
				if (currentFrame == boxWaitFrames / 2) {
					if (j < 2) {
						_input[i][j + 1].aniFrame = 0;
					}
				}
				// nothing
			}
			else if (currentFrame < boxWaitFrames + boxBounceFrames / 2) {
				// smol bounce
				dstRect->y -= (currentFrame - boxWaitFrames) * 1;
			}
			else {
				dstRect->y -= (boxBounceFrames + boxWaitFrames - currentFrame) * 1;
			}
			(_input[i][j].aniFrame)++;
			currentFrame = _input[i][j].aniFrame;
			if (currentFrame == boxBounceFrames + boxWaitFrames) {
				_input[i][j].aniFrame = -1;
			}
		}
		else {
			// decrease or increase by 4 pixels per frame
		// 3 frame difference per box
		// stop on frame 30
			if (_input[i][j].color[k] != 0) {
				if ((currentFrame) <= boxFlipFrames / 2) {
					dstRect->h = (dstRect->h - (int)(((float)120 / boxFlipFrames) * (currentFrame))) / 3;
					dstRect->y += (int)(((float)60 / boxFlipFrames) * (currentFrame)) + dstRect->h * k;
				}
				else {
					dstRect->h = (dstRect->h - (int)(((float)120 / boxFlipFrames) * (boxFlipFrames - (currentFrame)))) / 3;
					dstRect->y += (int)(((float)60 / boxFlipFrames) * (boxFlipFrames - (currentFrame))) + dstRect->h * k;
				}
			}
			else {
				if ((currentFrame) <= boxFlipFrames / 2) {
					dstRect->h -= (int)(((float)120 / boxFlipFrames) * (currentFrame));
					dstRect->y += (int)(((float)60 / boxFlipFrames) * (currentFrame));
				}
				else {
					dstRect->h -= (int)(((float)120 / boxFlipFrames) * (boxFlipFrames - (currentFrame)));
					dstRect->y += (int)(((float)60 / boxFlipFrames) * (boxFlipFrames - (currentFrame)));
				}
			}

			if (k == 2) {
				(_input[i][j].aniFrame)++;
				currentFrame = _input[i][j].aniFrame;
				if (currentFrame == (boxFlipFrames / 4) && j < 2) {
					_input[i][j + 1].aniFrame = 1;
				}
				else if (currentFrame == boxFlipFrames / 2) {
					for (int l = 0; l < 3; l++) {
						_input[i][j].color[l] = tmpColor[j][l];
					}
				}
				else if (currentFrame == boxFlipFrames) {
					_input[i][j].aniFrame = -1;
					if (j == 2) {
						checkAnswerFound();
						if (foundAnswer) {
							_input[i][0].aniFrame = 0;
						}
						else {
							isTypable = true;
							setAnswerData();
						}
					}
				}
			}
		}
	}
	else {
		if (_input[i][j].color[k] != 0) {
			dstRect->h /= 3;
			dstRect->y += 20 * k;
		}
	}
	if (j < 3) {
		// vs keeps warning about an event that never occurs ("j == 3")
		SDL_RenderCopy(_renderer, box[_input[i][j].color[k]], NULL, dstRect);
	}
	return 0;
}

void Kordle::renderText(SDL_Texture* _texture, SDL_Renderer* _renderer, SDL_Rect* dstRect, short* data, short* data2) {
	dstRect->w = dstRect->h = 60;
	if (textTexture == NULL) return;

	SDL_QueryTexture(textTexture[data2[0]][data2[1]], NULL, NULL, &(dstRect->w), &(dstRect->h));
	dstRect->x += (60 - dstRect->w) / 2;
	dstRect->y += (60 - dstRect->h) / 2 - 2;
	short currentFrame = _input[data2[0]][data2[1]].aniFrame;

	if (currentFrame != -1) {
		if (foundAnswer) {
			if (currentFrame < boxWaitFrames) {
				// nothing
			}
			else if (currentFrame < boxWaitFrames + boxBounceFrames / 2) {
				// smol bounce
				dstRect->y -= (currentFrame - boxWaitFrames) * 1;
			}
			else {
				dstRect->y -= (boxBounceFrames + boxWaitFrames - currentFrame) * 1;
			}
		}
		else {
			if (currentFrame <= boxFlipFrames / 2) {
				dstRect->h -= (int)(((float)120 / boxFlipFrames) * currentFrame);
				dstRect->y += (int)(((float)60 / boxFlipFrames) * currentFrame);
			}
			else {
				dstRect->h -= (int)(((float)120 / boxFlipFrames) * (boxFlipFrames - currentFrame));
				dstRect->y += (int)(((float)60 / boxFlipFrames) * (boxFlipFrames - currentFrame));
			}
		}
	}

	SDL_RenderCopy(_renderer, textTexture[data2[0]][data2[1]], NULL, dstRect);
}

// Updates the input texture(s).
void Kordle::drawText(Font* f, SDL_Renderer* _renderer, int type) {
	if (!type) return;
	int rk = findRK();
	SDL_DestroyTexture(textTexture[tries][rk / 3]);
	textTexture[tries][rk / 3] = f->getLetterTexture(textTexture[tries][rk / 3], _renderer, _input[tries][rk / 3].jamo);
	if (rk >= 3 && type == 1) {
		textTexture[tries][rk / 3 - 1] = f->getLetterTexture(textTexture[tries][rk / 3 - 1], _renderer, _input[tries][rk / 3 - 1].jamo);
	}
	else if (rk < 9 && type == -1) {
		textTexture[tries][rk / 3 + 1] = f->getLetterTexture(textTexture[tries][rk / 3 + 1], _renderer, _input[tries][rk / 3 + 1].jamo);
	}
}