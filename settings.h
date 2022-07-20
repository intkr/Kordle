#pragma once

#include <fstream>
#include <string>

class Settings {
public:
	Settings();
	~Settings();

	int getScrWidth();
	int getScrHeight();
	int getFPS();
	int getIconSize();
	int getOpenPopup();
	bool isMenuOpen();
	void setOpenPopup(int n);
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