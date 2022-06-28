#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <Windows.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "settings.h"
#include "graphics.h"
#include "font.h"

/* Kordle

Screen size : 400x650 (default), maybe consider 600x975 as an option

*/

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	TTF_Init();

	Settings* s = new Settings();
	Graphics* g = new Graphics(s);
	Font* f = new Font(g);

	SDL_Event _event;

	int winWidth = GetSystemMetrics(SM_CXSCREEN);
	int winHeight = GetSystemMetrics(SM_CYSCREEN);
	int gameStatus = 1;	// 0 : Stopped, 1 : Main screen, 2 : Menu opened



	while (gameStatus) {
		while (SDL_PollEvent(&_event)) {
			SDL_RenderClear(g->_renderer);

			g->renderScreen(f->getTitleTexture());

			SDL_RenderPresent(g->_renderer);
		}
	}
	
	return 0;
}