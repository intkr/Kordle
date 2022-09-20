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
void redraw(Settings* s, Graphics* g, Font* f, Input* i, Kordle* k, Popup* p);

extern char lang;
extern bool isDarkMode;

// console	: int main(int argc, char** argv)
// windows	: INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	TTF_Init();

	Settings* s = new Settings();
	Graphics* g = new Graphics(s);
	Font* f = new Font(g);
	Input* i = new Input();
	Kordle* k = new Kordle(g, f, s);
	Popup* p = new Popup(g, s);

	SDL_Event _event;

	int gameStatus = 1;
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

	if (s->debugMode) {
		ShowWindow(GetConsoleWindow(), SW_NORMAL);
		SDL_RaiseWindow(g->_window);
	}
	else {
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	while (gameStatus) {
		frameStartTime = SDL_GetTicks();
		while (SDL_PollEvent(&_event)) {
			switch (_event.type) {
				// mouse stuff
			case SDL_MOUSEBUTTONDOWN:
				if (p->openedBigPop) {
					handle = i->detectButton(mousePos, 2, g->getMenuRects());
					if (handle == -1 && p->openedBigPop == 202) {
						// settings popup specifically
						handle = p->_controls->detectSwitch(mousePos);
					}
				}
				else if (s->isMenuOpen()) {
					// menu
					handle = i->detectButton(mousePos, 1, g->getMenuRects());
				}
				else {
					// game
					handle = i->detectButton(mousePos, 0, g->getMenuRects());
				}
				printf("Mouse button (%d)\n", handle);
				switch (handle / 100) {
				case 0:
					if (gameStatus != 1) break;
					switch (handle % 100) {
					case 0: // Open/close menu
						s->switchMenuOpen();
						break;
					case 1: // Close window
					{
						int r[2] = { 0, };
						SDL_SetWindowHitTest(g->_window, NULL, mousePos);
						return 0;
						break;
					}
					case 2: // Open help popup
						p->openedBigPop = 201;
						p->drawPopup(g, f, k, 201);
						break;
					case 3: // Open stats popup
						p->openedBigPop = 200;
						p->drawPopup(g, f, k, 200);
						break;
					case 4: // Open settings popup
						p->openedBigPop = 202;
						p->drawPopup(g, f, k, 202);
						break;
					case 5: // Minimize window
						SDL_MinimizeWindow(g->_window);
						break;
					case 6: // Close popup
						p->closeBigPopup();
						break;
					case 10:
						if (isDarkMode)
							isDarkMode = false;
						else
							isDarkMode = true;
						printf("Dark mode switch triggered (%d)\n", isDarkMode);
						redraw(s, g, f, i, k, p);
						s->saveSettings();
						k->saveData();
						p->_controls->updateSwitch(handle - 10);
						break;
					case 11:
						if (lang == 'k')
							lang = 'u';
						else
							lang = 'k';
						printf("Language switch changed (%c)\n", lang);
						redraw(s, g, f, i, k, p);
						s->saveSettings();
						k->saveData();
						p->_controls->updateSwitch(handle - 10);
						break;
					case 12:
						if (s->debugMode) {
							ShowWindow(GetConsoleWindow(), SW_HIDE);
							s->debugMode = false;
						}
						else {
							ShowWindow(GetConsoleWindow(), SW_NORMAL);
							SDL_RaiseWindow(g->_window);
							s->debugMode = true;
						}
						printf("Debug mode triggered (%d)\n", s->debugMode);
						s->saveSettings();
						k->saveData();
						p->_controls->updateSwitch(handle - 10);
						break;
					}
					break;
				}
				break;
			case SDL_MOUSEMOTION:
				if (p->openedBigPop) {
					handle = i->detectButton(mousePos, 2, g->getMenuRects());
					if (handle == -1 && p->openedBigPop == 202) {
						// settings popup specifically
						handle = p->_controls->detectSwitch(mousePos);
					}
				}
				else if (s->isMenuOpen()) {
					// menu
					handle = i->detectButton(mousePos, 1, g->getMenuRects());
				}
				else {
					// game
					handle = i->detectButton(mousePos, 0, g->getMenuRects());
				}
				switch (handle / 100) {
				case 0:
					if (gameStatus != 1) break;
					if (handle > -1) {
						SDL_FreeCursor(_cursor);
						_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
						SDL_SetCursor(_cursor);
					}
					else {
						SDL_FreeCursor(_cursor);
						_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
						SDL_SetCursor(_cursor);
					}
					break;
				}
				break;
				// key stuff
			case SDL_KEYDOWN:
				key = i->handleKey(_event.key.keysym.sym, (SDL_Keymod)_event.key.keysym.mod);
				printf("Key press (%d)\n", key);
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
							if (handle == 999) {
								// save
								k->saveData();
								break;
							}
							// popup
							if (handle >= 200) {
								p->openedBigPop = 200;
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
				p->openedBigPop = k->delayedPopup;
				p->drawPopup(g, f, k, k->delayedPopup);
				k->delayedPopup = 0;
			}
		}
		SDL_GetMouseState(&mousePos[0], &mousePos[1]);

		// Rendering
		SDL_RenderClear(g->_renderer);
		const char* errorMsg = SDL_GetError();
		if (strcmp(errorMsg, "\0") == 0) {
			printf("%s\n", errorMsg);
			SDL_ClearError();
		}
		g->renderScreen(f->getTitleTexture(), s);
		k->renderGame(f, g);
		p->renderPopup(g, f);
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
		if ((pos[1] <= 50 ) && (pos[0] > 34 ) && (pos[0] < 335 )) {
			r = SDL_HITTEST_DRAGGABLE;
		}
	}
	return r;
}

void redraw(Settings* s, Graphics* g, Font* f, Input* i, Kordle* k, Popup* p) {
	g->reset(s);
	f->reset(g);
	p->_controls->reset(g, s->debugMode);
	p->reset();			// color
	p->reset2(g, f, k); // popup
	k->reset(f, g->_renderer);
}