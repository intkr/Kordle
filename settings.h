#pragma once

#include <stdio.h> // printf testing
#include <fstream>
#include <string>

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

	int menuAnimationNo;
	int popupAnimationNo;

private:
	int scrWidth;
	int scrHeight;
	int fps;
	int iconSize;
	int openPopup;
	bool openMenu;
};