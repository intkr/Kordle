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
#include "popup.h"

/*		Kordle - Korean version of Kordle
						   Github : intkr
*/

SDL_HitTestResult hitTest(SDL_Window* win, const SDL_Point* area, void* data);

int main(int argc, char** argv) {
	//printf("%d\n", time(NULL) / 86400);

	// ShowWindow(GetConsoleWindow(), HIDE_WINDOW);
	// ShowWindow(GetConsoleWindow(), SHOW_OPENWINDOW);
	// SDL_RaiseWindow( sdl window* here );
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	TTF_Init();

	Settings* s = new Settings();
	Graphics* g = new Graphics(s);
	Font* f = new Font(g);
	Input* i = new Input();
	Kordle* k = new Kordle();
	Popup* p = new Popup();

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

	if (!k->getPlayerData(0) && !k->getPlayerData(1)) {
		k->delayedPopup = 201;
		k->popupDelayFrames = 0;
	}
	while (gameStatus) {
		frameStartTime = SDL_GetTicks();
		while (SDL_PollEvent(&_event)) {
			switch (_event.type) {
			// mouse stuff
			case SDL_MOUSEBUTTONDOWN:
				handle = i->handleClick(i->detectButton(mousePos, p->isBigPopOpen ? 2 : s->isMenuOpen() ? 1 : 0, g->getMenuRects()));
				switch (handle / 100) {
				case 0:
					if (gameStatus != 1) break;
					switch (handle % 100) {
					case 1: // Open/close menu
						s->switchMenuOpen();
						break;
					case 2: // Minimize window
						SDL_MinimizeWindow(g->_window);
						break;
					case 3: // Open stats popup
						p->isBigPopOpen = true;
						p->drawPopup(g, f, k, 200);
						break;
					case 4: // Close window
					{
						int r[2] = { 0, };
						SDL_SetWindowHitTest(g->_window, NULL, mousePos);
						return 0;
						break;
					}
					case 5: // Close popup
						p->closeBigPopup();
						break;
					case 6: // Open help popup
						p->isBigPopOpen = true;
						p->drawPopup(g, f, k, 201);
						break;
					case 7: // Open settings popup
						p->isBigPopOpen = true;
						p->drawPopup(g, f, k, 202);
						break;
					}
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				handle = i->handleClick(i->detectButton(mousePos, p->isBigPopOpen ? 2 : s->isMenuOpen() ? 1 : 0, g->getMenuRects()));
				switch (handle / 100) {
				case 0:
					if (gameStatus != 1) break;
					switch (handle % 100) {
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
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
				if (key == -1) {

				}
				else if (key >= 100) {
					// game-unrelated keys
					switch (key - 100) {
					case 0:
						s->switchMenuOpen();
						break;
					default:
						break;
					}
				}
				else {
					// game-related keys
					if (k->isTypable) {
						handle = k->handleInput(key);
						if (handle >= 100) {
							// popup
							if (handle >= 200) {
								p->isBigPopOpen = true;
							}
							else {
								p->smallPopFrames = s->getFPS() * 3;
							}
							p->drawPopup(g, f, k, handle);
							k->drawText(f, g->_renderer, 0);
						}
						else {
							k->drawText(f, g->_renderer, handle);
						}
					}
				}
				break;
			}
		}
		
		// temporary way to trigger popups
		if (k->popupDelayFrames) {
			k->popupDelayFrames--;
		}
		else {
			if (k->delayedPopup) {
				p->isBigPopOpen = true;
				p->drawPopup(g, f, k, k->delayedPopup);
				k->delayedPopup = 0;
			}
		}
		SDL_GetMouseState(&mousePos[0], &mousePos[1]);

		// Rendering
		SDL_RenderClear(g->_renderer);
		g->renderScreen(f->getTitleTexture(), s);
		k->renderGame(f, g);
		p->renderPopup(g);
		SDL_RenderPresent(g->_renderer);

		// FPS limiting
		frameSleepTime = (1000 / s->getFPS()) - int(SDL_GetTicks() - frameStartTime);
		if (frameSleepTime > 0)
			SDL_Delay(frameSleepTime);
	}

	delete(p);
	delete(g);
	delete(s);
	delete(i);
	delete(k);

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