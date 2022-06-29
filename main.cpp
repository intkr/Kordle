#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <Windows.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "settings.h"
#include "graphics.h"
#include "font.h"
#include "input.h"

/*		Kordle - Korean version of Kordle
						   Github : intkr
*/

SDL_HitTestResult hitTest(SDL_Window* win, const SDL_Point* area, void* data);

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	TTF_Init();

	Settings* s = new Settings();
	Graphics* g = new Graphics(s);
	Font* f = new Font(g);
	Input* i = new Input();

	SDL_Event _event;

	int winWidth = GetSystemMetrics(SM_CXSCREEN);
	int winHeight = GetSystemMetrics(SM_CYSCREEN);
	int gameStatus = 1;	// 0 : Stopped, 1 : Main screen, 2 : Menu opened
	bool mouseDown = false;
	int mousePos[] = { 0, };
	unsigned int frameStartTime;
	int frameSleepTime = 0;
	int handle = -1;

	SDL_SetWindowHitTest(g->_window, hitTest, mousePos);

	while (gameStatus) {
		frameStartTime = SDL_GetTicks();
		while (SDL_PollEvent(&_event)) {
			switch (_event.type) {
			case SDL_MOUSEBUTTONDOWN:
				handle = i->handleClick(i->detectClickedButton(mousePos, 0, g->getMenuRects()));
				switch (handle / 100) {
				case 0:
					switch (handle % 100) {
					case 1:
						s->switchMenuOpen();
						break;
					}
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				break;
			}
		}
		SDL_GetMouseState(&mousePos[0], &mousePos[1]);

		// Drawing
		SDL_RenderClear(g->_renderer);
		g->renderScreen(f->getTitleTexture(), s);
		SDL_RenderPresent(g->_renderer);

		// Caps game at 60 FPS
		frameSleepTime = (1000 / s->getFPS()) - int(SDL_GetTicks() - frameStartTime);
		if (frameSleepTime > 0)
			SDL_Delay(frameSleepTime);
	}
	
	return 0;
}

SDL_HitTestResult hitTest(SDL_Window* win, const SDL_Point* area, void* data) {
	SDL_HitTestResult r = SDL_HITTEST_NORMAL;
	int* pos = (int*)data;
	short test = GetAsyncKeyState(VK_LBUTTON);
	if (test != 0) {
		if ((pos[1] <= 50) && (pos[0] > 34) && (pos[0] <= 400)) {
			r = SDL_HITTEST_DRAGGABLE;
		}
	}
	return r;
}