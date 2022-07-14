#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <Windows.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "settings.h"
#include "graphics.h"
#include "font.h"
#include "input.h"
#include "kordle.h"

/*		Kordle - Korean version of Kordle
						   Github : intkr
*/

SDL_HitTestResult hitTest(SDL_Window* win, const SDL_Point* area, void* data);

int main(int argc, char** argv) {
	ShowWindow(GetConsoleWindow(), SHOW_OPENWINDOW);
	//ShowWindow(GetConsoleWindow(), HIDE_WINDOW);	// Don't use this when testing new stuff
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	TTF_Init();

	Settings* s = new Settings();
	Graphics* g = new Graphics(s);
	Font* f = new Font(g);
	Input* i = new Input();
	Kordle* k = new Kordle();

	SDL_Event _event;

	int winWidth = GetSystemMetrics(SM_CXSCREEN);
	int winHeight = GetSystemMetrics(SM_CYSCREEN);
	int gameStatus = 1;
	bool mouseDown = false;
	int mousePos[2] = { 0, };
	unsigned int frameStartTime;
	int frameSleepTime = 0;
	int handle = -1;
	int key = -1;
	SDL_Cursor* _cursor = SDL_GetCursor();

	SDL_SetWindowHitTest(g->_window, hitTest, mousePos);

	while (gameStatus) {
		frameStartTime = SDL_GetTicks();
		while (SDL_PollEvent(&_event)) {
			switch (_event.type) {
			// mouse stuff
			case SDL_MOUSEBUTTONDOWN:
				handle = i->handleClick(i->detectButton(mousePos, 0, g->getMenuRects()));
				switch (handle / 100) {
				case 0:
					if (gameStatus != 1) break;
					switch (handle % 100) {
					case 1:
						s->switchMenuOpen();
						break;
					case 2:
						SDL_MinimizeWindow(g->_window);
						break;
					case 4:
						int r[2] = { 0, };
						SDL_SetWindowHitTest(g->_window, NULL, mousePos);
						return 0;
						break;
					}
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				handle = i->handleClick(i->detectButton(mousePos, 0, g->getMenuRects()));
				switch (handle / 100) {
				case 0:
					if (gameStatus != 1) break;
					switch (handle % 100) {
					case 1:
					case 2:
					case 4:
						_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
						SDL_SetCursor(_cursor);
						break;
					case -1:
					default:
						_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
						SDL_SetCursor(_cursor);
						break;
					}
					break;
				}
				break;
			// key stuff
			case SDL_KEYDOWN:
				key = i->handleKey(_event.key.keysym.sym, (SDL_Keymod)_event.key.keysym.mod);
				printf("%d\n", key);
				if (key == -1) {

				}
				else if (key >= 99) { /// should be 100 but I didn't code for Enter yet
					// game-unrelaed keys
				}
				else {
					// game-related keys
					k->handleInput(key);
				}
				break;
			}
		}
		SDL_GetMouseState(&mousePos[0], &mousePos[1]);

		// Rendering
		SDL_RenderClear(g->_renderer);
		g->renderScreen(f->getTitleTexture(), s);
		k->renderGame(f, g);
		SDL_RenderPresent(g->_renderer);

		// FPS limiting
		frameSleepTime = (1000 / s->getFPS()) - int(SDL_GetTicks() - frameStartTime);
		if (frameSleepTime > 0)
			SDL_Delay(frameSleepTime);
	}

	f->~Font();
	g->~Graphics();
	s->~Settings();
	TTF_Quit();
	SDL_Quit();
	return 0;
}

SDL_HitTestResult hitTest(SDL_Window* win, const SDL_Point* area, void* data) {
	SDL_HitTestResult r = SDL_HITTEST_NORMAL;
	int* pos = (int*)data;
	short test = GetAsyncKeyState(VK_LBUTTON);
	if (test != 0) {
		if ((pos[1] <= 50) && (pos[0] > 34) && (pos[0] < 335)) {
			r = SDL_HITTEST_DRAGGABLE;
		}
	}
	return r;
}