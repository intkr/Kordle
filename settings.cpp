#include "settings.h"

Settings::Settings() {
	if (false) {
		// Apply settings from save file
	}
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

void Settings::setOpenPopup(int n) {
	openPopup = n;
}

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