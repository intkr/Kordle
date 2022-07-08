#pragma once

#include <fstream>
#include <string>
#include <stdlib.h>

class Box {
public:
	wchar_t jamo[3];
	unsigned short color[3];
};

class Kordle {
public:
	Kordle();
	~Kordle();

	unsigned int tries;
private:
	Box input[6][4];
	wchar_t answer[4][3];

	unsigned int playedGames;
	unsigned int maxStreak;
	unsigned int currentStreak;
	unsigned int gamesWon[6];
	unsigned int totalWon;
	unsigned int winrate;
	float avgPlacement;
};