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

SDL_HitTestResult hitTest(SDL_Window* win, const SDL_Point* area, void* data);

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
	bool mouseDown = false;
	int idk[] = { 0, };
	unsigned int frameStartTime;
	int frameSleepTime = 0;

	SDL_SetWindowHitTest(g->_window, hitTest, idk);

	while (gameStatus) {
		frameStartTime = SDL_GetTicks();
		while (SDL_PollEvent(&_event)) {
			switch (_event.type) {
			case SDL_MOUSEMOTION:
				break;
			}
		}
		SDL_GetMouseState(&idk[0], &idk[1]);

		SDL_RenderClear(g->_renderer);
		g->renderScreen(f->getTitleTexture());
		SDL_RenderPresent(g->_renderer);
		frameSleepTime = (1000 / s->fps) - int(SDL_GetTicks() - frameStartTime);
		if (frameSleepTime > 0)
			SDL_Delay(frameSleepTime);
	}
	
	return 0;
}

SDL_HitTestResult hitTest(SDL_Window* win, const SDL_Point* area, void* data) {
	SDL_HitTestResult r = SDL_HITTEST_NORMAL;
	int* aaa = (int*)data;
	short test = GetAsyncKeyState(VK_LBUTTON);
	if (test != 0) {
		if ((aaa[1] <= 50)) {
			r = SDL_HITTEST_DRAGGABLE;
		}
	}
	return r;
}