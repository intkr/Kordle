#pragma once

#include <stdio.h> // printf testing

class Settings {
public:
	Settings();
	~Settings();

	// Returns the width of the game screen by pixels.
	int getScrWidth();

	// Returns the height of the game screen by pixels.
	int getScrHeight();

	// Returns FPS.
	int getFPS();

	// Returns the size of icons by pixels.
	// All icons are drawn within the same rectangular size.
	int getIconSize();

	// Returns what pop-up is currently open.
	// Refer to notes.txt for details.
	int getOpenPopup();

	// Sets what pop-up is currently open.
	// Refer to notes.txt for details.
	void setOpenPopup(int n);

	// Returns whether or not the menu is open in boolean form.
	bool isMenuOpen();

	// Switches the boolean value of openMenu.
	void switchMenuOpen();

	int menuAnimationNo = -1;
	int popupAnimationNo = -1;

private:
	int scrWidth = 400;
	int scrHeight = 650;
	int fps = 60;
	int iconSize = 20;
	int openPopup = -1;
	bool openMenu = false;
};