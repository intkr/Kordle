#pragma once

#include <fstream>
#include <string>
#include <map>
#include <unordered_set>
#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

#include "font.h"
#include "graphics.h"

class Box {
public:
	short jamo[3];
	short color[3];
	short aniFrame;
};

class Kordle {
public:
	Kordle();
	~Kordle();
	short renderBox(SDL_Renderer* _renderer, SDL_Rect* dstRect, SDL_Texture** box, short* data);
	void renderText(SDL_Texture* _texture, SDL_Renderer* _renderer, SDL_Rect* dstRect, short* data, short* data2);
	void renderGame(Font* f, Graphics* g);
	void drawText(Font* f, SDL_Renderer* _renderer, int type);
	int handleInput(int key);

	bool isTypable;
private:
	int findRK();
	void checkAnswer();
	void checkAnswerFound();
	bool checkCombinedJamo(int i, int j);
	short getJamoData(short data, int type);
	void setAnswerData();
	bool validateAnswer();

	Box _input[6][3];
	unsigned short answer[3][3];
	unsigned short tmpColor[3][3];
	SDL_Texture* textTexture[6][3];
	bool foundAnswer; // probably unnecessary

	std::map<short, int> answerData;
	std::map<short, int> inputData;
	std::unordered_set<std::wstring> wordList;

	short rk;

	// user save data
	unsigned int tries;
	unsigned int playedGames;
	unsigned int maxStreak;
	unsigned int currentStreak;
	unsigned int gamesWon[6];
	unsigned int totalWon;
	unsigned int winrate;
	float avgPlacement;

	const int boxFlipFrames = 30;
	const int boxWaitFrames = 20;
	const int boxBounceFrames = 40; // 30 frames of waiting + 30 frames of animation
};