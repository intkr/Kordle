#include "kordle.h"

extern char lang;
extern bool isDarkMode;

Kordle::Kordle(Graphics* g, Font* f, Settings* s) {
	popupDelayFrames = 0;
	delayedPopup = 0;
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
	int tmp[28] = { 0, };
	std::wifstream wreader;
	std::wstring wline;

	wreader.open("assets/list.csv", std::wifstream::in);
	while (wreader.good()) {
		std::getline(wreader, wline);
		wordList.insert(wline);
	}
	wreader.close();

	wreader.open("assets/data.txt", std::wifstream::in);
	if (wreader.good()) {
		for (int i = 0; i < _countof(tmp); i++) {
			std::getline(wreader, wline);
			tmp[i] = _wtoi(wline.c_str());
		}
	}
	else {
		printf("Data save file didn't load properly");
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

	if (tries == 6) isTypable = false;
	else isTypable = true;


	wreader.open("assets/answer.csv", std::wifstream::in);
	bool flag = false;
	int newDay = 0;
	if (wreader.good()) {
		// skip previous answers

		//(signed int)time(NULL) / 86400 - releaseDateUNIX - 1
		if (s->lastDay) {
			printf("Last day was %d, ", s->lastDay - releaseDateUNIX);
		}
		else {
			printf("First time playing, ");
		}

		// reset input if time(NULL) / 86400 > s->lastDay, and add a loss on saveData
		// don't do above if s->lastDay is 0, because that means today is the first day
		if (s->lastDay == 0) {
			newDay = 2; // first day
		}
		else if (s->lastDay < time(NULL) / 86400) {
			newDay = 1; // new day
		}
		printf("today is Day %d (%d)\n", (signed int)time(NULL) / 86400 - releaseDateUNIX, newDay);

		s->lastDay = (signed int)time(NULL) / 86400;
		s->saveSettings();
		for (int i = 0; i <= s->lastDay - releaseDateUNIX - 1; i++) {
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
		// resets to '고장남'
		answer[0][0] = 0;
		answer[0][1] = 8;
		answer[1][0] = 12;
		answer[1][1] = 0;
		answer[1][2] = 21;
		answer[2][0] = 2;
		answer[2][1] = 0;
		answer[2][2] = 16;
	}

	setAnswerData();

	switch (newDay) {
	case 2: // first (new) day
		// nothing to do for now
		break;
	case 1: // new day
		// if 0 < tries < 6, game ended prematurely; count as loss
		// either case of losing / winning on last turn (previous day) leads to saveData (loss / win), so don't have to update data in this case
		if (tries > 0 && tries < 6) {
			updatePlayerData(2);
		}
		break;
	case 0: // same day
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 3; j++) {
				if (tmp[10 + i * 3 + j] == 0) continue;
				tmp[10 + i * 3 + j] -= 44032;
				_input[i][j].jamo[0] = tmp[10 + i * 3 + j] / 588;
				_input[i][j].jamo[1] = (tmp[10 + i * 3 + j] % 588) / 28;
				_input[i][j].jamo[2] = tmp[10 + i * 3 + j] % 28;
			}
		}

		if (tries > 0) {
			unsigned int tmpTries = tries;
			for (unsigned int i = 0; i < tmpTries; i++) {
				tries = i;
				checkAnswer();
				//_input[i][0].aniFrame = 1;
				for (int j = 0; j < 3; j++) {
					for (int k = 0; k < 3; k++) {
						_input[i][j].color[k] = tmpColor[j][k];
					}
				}
				setAnswerData();
			}
			tries = tmpTries;
			reset(f, g->_renderer);
			checkAnswerFound();
		}

		if (foundAnswer) {
			isTypable = false;
			popupDelayFrames = 0;
			delayedPopup = 200;
		}
		break;
	default:
		// shouldn't happen
		break;
	}
	
	wreader.close();
}

Kordle::~Kordle() {
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 4; j++) {
			SDL_DestroyTexture(textTexture[i][j]);
		}
	}
}

void Kordle::saveData() {
	int stuff[28] = { (int)tries, (int)playedGames, (int)maxStreak, (int)currentStreak, 0, };
	char tmp[6] = { 0, };

	for (int i = 0; i < 6; i++) {
		stuff[4 + i] = gamesWon[i];
		if (_input[i][0].jamo[0] == -1) continue;
		for (int j = 0; j < 3; j++) {
			stuff[10 + i * 3 + j] = _input[i][j].jamo[0] * 588 + _input[i][j].jamo[1] * 28 + _input[i][j].jamo[2] + 44032;
		}
	}

	std::ofstream writer;
	writer.open("assets/data.txt", std::ofstream::out);
	if (writer.good()) {
		for (int i = 0; i < _countof(stuff); i++) {
			_itoa_s(stuff[i], tmp, 10);
			writer.write(tmp, strlen(tmp));
			writer.write("\n\0", strlen("\n\0"));
		}
	}
	else {
		printf("Data save file didn't save properly");
	}
	writer.close();
}

void Kordle::updatePlayerData(int n) {
	printf("Data updated (%d)\n", n);
	switch (n) {
	case 0: // lost
		playedGames++;
		currentStreak = 0;
		winrate = (int)((float)totalWon / playedGames * 100);
		tries = 6;
		break;
	case 1: // won
		playedGames++;
		gamesWon[tries - 1]++;
		currentStreak++;
		if (currentStreak > maxStreak) maxStreak = currentStreak;
		totalWon++;
		winrate = (int)((float)totalWon / playedGames * 100);
		break;
	case -1: // tried
		tries++;
		break;
	case 2: // lost yesterday (only for new day)
		playedGames++;
		currentStreak = 0;
		winrate = (int)((float)totalWon / playedGames * 100);
		tries = 0;
		break;
	default:
		// shouldn't happen
		break;
	}
	saveData();
}

unsigned int Kordle::getPlayerData(int n) {
	if (n >= 10) return gamesWon[n - 10];
	switch (n) {
	case 0:
		return tries;
	case 1:
		return playedGames;
	case 2:
		return winrate;
	case 3:
		return currentStreak;
	case 4:
		return maxStreak;
	}
	return -1; // shouldn't happen
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

// Checks if input is a legitimate word.
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
// (other) : No updates, signal popup
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
		// enter
		if (_input[tries][2].jamo[1] == -1) {
			return 100;
		}
		if (validateAnswer()) {
			checkAnswer();
			_input[tries][0].aniFrame = 1;
			updatePlayerData(-1);
			isTypable = false;
		}
		else {
			return 101;
		}
		return 999;
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

// Checks if current input is the answer,
// and sets box colors accordingly.
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

	int tmpJamo = -1;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (tmpColor[i][j] == 4 || tmpColor[i][j] == 2) continue;
			tmpJamo = checkCombinedJamo(i, j);
			if (tmpJamo != -1) {
				// Olive
				if (answerData[tmpJamo] > 0) {
					tmpColor[i][j] = 3;
					answerData[tmpJamo] -= 1;
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

// Checks if answer is correct.
// Controls the value of foundAnswer accordingly.
void Kordle::checkAnswerFound() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (tmpColor[i][j] != 4) {
				return;
			}
		}
	}
	printf("Correct answer");
	foundAnswer = true;
}

int Kordle::checkCombinedJamo(int i, int j) {
	short tmp = _input[tries][i].jamo[j];
	switch (j) {
	case 0:
		return -1;
	case 1:
		// vowel
		for (int k = 0; k < 3; k++) {
			switch (answer[k][j]) {
			case 9:
				if (tmp == 0 || tmp == 8)
					return 59;
				break;
			case 10:
				if (tmp == 1 || tmp == 8)
					return 60;
				break;
			case 11:
				if (tmp == 20 || tmp == 8)
					return 61;
				break;
			case 14:
				if (tmp == 4 || tmp == 13)
					return 64;
				break;
			case 15:
				if (tmp == 5 || tmp == 13)
					return 65;
				break;
			case 16:
				if (tmp == 20 || tmp == 13)
					return 66;
				break;
			case 19:
				if (tmp == 20 || tmp == 18)
					return 69;
				break;
			}
		}
		return -1;
	case 2:
		// final consonant
		for (int k = 0; k < 3; k++) {
			switch (answer[k][2]) {
			case 3:
				if (tmp == 1 || tmp == 19)
					return 103;
				break;
			case 5:
				if (tmp == 4 || tmp == 22)
					return 105;
				break;
			case 9:
				if (tmp == 1 || tmp == 8)
					return 109;
				break;
			case 10:
				if (tmp == 16 || tmp == 8)
					return 110;
				break;
			case 11:
				if (tmp == 17 || tmp == 8)
					return 111;
				break;
			case 12:
				if (tmp == 19 || tmp == 8)
					return 112;
				break;
			case 13:
				if (tmp == 25 || tmp == 8)
					return 113;
				break;
			case 14:
				if (tmp == 26 || tmp == 8)
					return 114;
				break;
			case 15:
				if (tmp == 27 || tmp == 8)
					return 115;
				break;
			case 18:
				if (tmp == 17 || tmp == 19)
					return 118;
				break;
			}
		}
	default:
		// shouldn't happen
		return -1;
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
			boxRect.x = (100 + j * 70);
			data[0] = i;
			data[1] = j;
			for (int k = 0; k < 3; k++) {
				boxRect.y = (100 + i * 70);
				data[2] = k;
				renderBox(f, g->_renderer, &boxRect, g->getBoxSprites(), data);
			}

			boxRect.y = (100 + i * 70);
			for (int k = 0; k < 3; k++) {
				data[k] = _input[i][j].jamo[k];
			}
			data2[0] = i, data2[1] = j;
			if (textTexture[i][j] != NULL)
				renderText(textTexture[i][j], g->_renderer, f, &boxRect, data, data2);
		}
		
	}
}

void Kordle::renderBox(Font* f, SDL_Renderer* _renderer, SDL_Rect* dstRect, SDL_Texture** box, short* data) {
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
			if (currentFrame == boxBounceFrames + boxWaitFrames) { // end of animation
				_input[i][j].aniFrame = -1;
				if (j == 2) {
					printf("Triggering popup and resetting text textures");
					updatePlayerData(1);
					popupDelayFrames = 60;
					delayedPopup = 200;
					reset(f, _renderer);
				}
			}
		}
		else {
			// decrease or increase by 4 pixels per frame
		// 3 frame difference per box
		// stop on frame 30
			if (_input[i][j].color[k] != 0) {
				if ((currentFrame) <= boxFlipFrames / 2) {
					dstRect->h = (dstRect->h - (int)(((float)120 / boxFlipFrames) * (currentFrame))) / 3;
					dstRect->y += ((int)(((float)60 / boxFlipFrames) * (currentFrame)) + dstRect->h * k);
				}
				else {
					dstRect->h = (dstRect->h - (int)(((float)120 / boxFlipFrames) * (boxFlipFrames - (currentFrame)))) / 3;
					dstRect->y += ((int)(((float)60 / boxFlipFrames) * (boxFlipFrames - (currentFrame))) + dstRect->h * k);
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
				else if (currentFrame == boxFlipFrames) { // end of animation
					_input[i][j].aniFrame = -1;
					if (j == 2) {
						checkAnswerFound();
						if (foundAnswer) {
							_input[i][0].aniFrame = 0;
						}
						else {
							if (tries == 6) {
								// lost game
								updatePlayerData(0);
								popupDelayFrames = 60;
								delayedPopup = 200;
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
}

void Kordle::renderText(SDL_Texture* _texture, SDL_Renderer* _renderer, Font* f, SDL_Rect* dstRect, short* data, short* data2) {
	if (textTexture == NULL) return;

	SDL_QueryTexture(textTexture[data2[0]][data2[1]], NULL, NULL, &(dstRect->w), &(dstRect->h));
	dstRect->x += (60  - dstRect->w) / 2;
	dstRect->y += (60  - dstRect->h) / 2 - 2;
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
				if (currentFrame == boxFlipFrames / 2) {
					redrawText(f, _renderer, data2[1]);
				}
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
	textTexture[tries][rk / 3] = f->getLetterTexture(textTexture[tries][rk / 3], _renderer, _input[tries][rk / 3].jamo, _input[tries][rk / 3].color[0] ? 1 : 0);
	if (rk >= 3 && type == 1) {
		textTexture[tries][rk / 3 - 1] = f->getLetterTexture(textTexture[tries][rk / 3 - 1], _renderer, _input[tries][rk / 3 - 1].jamo, _input[tries][rk / 3].color[0] ? 1 : 0);
	}
	else if (rk < 9 && type == -1) {
		textTexture[tries][rk / 3 + 1] = f->getLetterTexture(textTexture[tries][rk / 3 + 1], _renderer, _input[tries][rk / 3 + 1].jamo, _input[tries][rk / 3].color[0] ? 1 : 0);
	}
}

void Kordle::redrawText(Font* f, SDL_Renderer* _renderer, int i) {
	SDL_DestroyTexture(textTexture[tries - 1][i]);
	textTexture[tries - 1][i] = f->getLetterTexture(textTexture[tries - 1][i], _renderer, _input[tries - 1][i].jamo, _input[tries - 1][i].color[0] ? 1 : 0);
}

void Kordle::reset(Font* f, SDL_Renderer* _renderer) {
	// redraw all box texts
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 3; j++) {
			SDL_DestroyTexture(textTexture[i][j]);
			textTexture[i][j] = f->getLetterTexture(textTexture[i][j], _renderer, _input[i][j].jamo, _input[i][j].color[0] ? 1 : 0);
		}
	}
}