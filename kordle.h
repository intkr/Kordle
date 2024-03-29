﻿#pragma once

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
	Kordle(Graphics* g, Font* f, Settings* s);
	~Kordle();
	void renderBox(Font* f, SDL_Renderer* _renderer, SDL_Rect* dstRect, SDL_Texture** box, short* data);
	void renderText(SDL_Texture* _texture, SDL_Renderer* _renderer, Font* f, SDL_Rect* dstRect, short* data, short* data2);
	void renderGame(Font* f, Graphics* g);
	void drawText(Font* f, SDL_Renderer* _renderer, int type);
	void redrawText(Font* f, SDL_Renderer* _renderer, int i);
	int handleInput(int key);
	void reset(Font* f, SDL_Renderer* _renderer);
	void saveData();

	// 10~15 : gamesWon		0 : tries
	// 1 : playedGames		2 : maxStreak
	// 3 : currentStreak	4 : winrate
	unsigned int getPlayerData(int n);
	
	void updatePlayerData(int n); // only happens when today's game ends

	bool isTypable;
	bool foundAnswer;

	int popupDelayFrames;
	int delayedPopup;

private:
	int findRK();
	void checkAnswer();
	void checkAnswerFound();
	int checkCombinedJamo(int i, int j);
	short getJamoData(short data, int type);
	void setAnswerData();
	bool validateAnswer();

	Box _input[6][3];
	unsigned short answer[3][3];
	unsigned short tmpColor[3][3];
	SDL_Texture* textTexture[6][3];

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

	const int releaseDateUNIX = 19255;
};