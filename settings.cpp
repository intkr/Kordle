#include "settings.h"

char lang = 'u';

Settings::Settings() {
	int stuff[8] = { 400, 650, 60, 20, -1, 0, -1, -1 };
	std::wifstream wreader;
	std::wstring wline;
	wreader.open("assets/settings.txt", std::wifstream::in);
	if (wreader.good()) {
		// Apply settings from save file
		for (int i = 0; i < _countof(stuff); i++) {
			std::getline(wreader, wline);
			stuff[i] = _wtoi(wline.c_str());
		}
	}
	scrWidth = stuff[0];
	scrHeight = stuff[1];
	fps = stuff[2];
	iconSize = stuff[3];
	openPopup = stuff[4];
	openMenu = stuff[5];
	menuAnimationNo = stuff[6];
	popupAnimationNo = stuff[7];
	//lang = 'k';
}

Settings::~Settings() {}

int Settings::getFPS() {
	return fps;
}

int Settings::getIconSize() {
	return iconSize;
}

int Settings::getScrHeight() {
	return scrHeight;
}

int Settings::getScrWidth() {
	return scrWidth;
}

bool Settings::isMenuOpen() {
	return openMenu;
}

int Settings::getOpenPopup() {
	return openPopup;
}

// Refer to notes.txt for return value details
void Settings::setOpenPopup(int n) {
	openPopup = n;
}

// Switches the boolean 'openMenu',
// and sets the frame number for the menu moving animation.
void Settings::switchMenuOpen() {
	if (openMenu) {
		openMenu = false;
		if (menuAnimationNo == -1)
			menuAnimationNo = 11;
	}
	else {
		openMenu = true;
		if (menuAnimationNo == -1)
			menuAnimationNo = 0;
	}
}