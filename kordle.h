#pragma once

#include <fstream>
#include <string>
#include <Windows.h>

#include "font.h"
#include "graphics.h"

class Box {
public:
	short jamo[3];
	short color[3];
};

class Kordle {
public:
	Kordle();
	~Kordle();
	void renderBox(SDL_Renderer* _renderer, SDL_Rect* dstRect, int color, SDL_Texture** box);
	void renderText(Font* f, SDL_Renderer* _renderer, SDL_Rect* dstRect, short* data);
	void renderGame(Font* f, Graphics* g);

	unsigned int tries;
private:
	Box input[6][4];
	unsigned short answer[4][3];

	unsigned int playedGames;
	unsigned int maxStreak;
	unsigned int currentStreak;
	unsigned int gamesWon[6];
	unsigned int totalWon;
	unsigned int winrate;
	float avgPlacement;
};