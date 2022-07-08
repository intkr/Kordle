#include "kordle.h"

Kordle::Kordle() {
	// initialize input to 0
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 6; j++) {
			for (int k = 0; k < 3; k++) {
				input[i][j].color[k] = 0;
				input[i][j].jamo[k] = 0;
			}
		}
	}
	
	std::string line;
	std::ifstream reader;	
	reader.open("assets/data.txt", std::ifstream::in);
	int tmp[10] = { 0, };
	if (reader.good()) {
		// file exists, read and initialize
		// 
		// make sure to check every single line if it's legit
		// ex) early eof, different types of values
		for (int i = 0; i < _countof(tmp); i++) {
			std::getline(reader, line);
			try {
				tmp[i] = atoi(line.c_str());
			}
			catch (...) {
				// if the save file is (partially) corrupted,
				// then the corresponding variables are initialized as 0
			}
		}
	}
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

	reader.open("assets/list.txt", std::ifstream::in);
	if (reader.good()) {
		try {
			for (int i = 0; i < tries - 1; i++) {
				std::getline(reader, line);
			}
			std::getline(reader, line); // actual answer for today
			
		}
		catch (...) {
			// any error would indicate a lack of words to initialize as an answer
			// so if such event happens, initialize answer as default
		}
	}
	reader.close();
}

Kordle::~Kordle() {

}