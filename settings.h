#pragma once

#include <fstream>
#include <string>

class Settings {
public:
	Settings();
	~Settings();

	void saveSettings();
	int getFPS();
	int getIconSize();
	bool isMenuOpen();
	void switchMenuOpen();

	int menuAnimationNo;
	int popupAnimationNo;
	bool debugMode;
	int lastDay;
private:
	int fps;
	int iconSize;
	bool openMenu;
};