#include "settings.h"

char lang = 'k'; // k : 107, u : 117
bool isDarkMode = true;

Settings::Settings() {
	int stuff[6] = { 60, 20, 107, 1, 1, 0};
	std::ifstream reader;
	std::string line;
	reader.open("assets/settings.txt", std::ifstream::in);
	if (reader.good()) {
		// Apply settings from save file
		for (int i = 0; i < _countof(stuff); i++) {
			std::getline(reader, line);
			stuff[i] = atoi(line.c_str());
		}
	}
	fps = stuff[0];
	iconSize = stuff[1];
	lang = stuff[2];
	isDarkMode = stuff[3];
	debugMode = stuff[4];
	lastDay = stuff[5];

	openMenu = false;
	menuAnimationNo = -1;
	popupAnimationNo = -1;

	reader.close();
}

Settings::~Settings() {}

void Settings::saveSettings() {
	// settings.txt
	int stuff[6] = { fps, iconSize, lang, isDarkMode, debugMode, lastDay };
	char tmp[6] = { 0, };
	std::ofstream writer;
	writer.open("assets/settings.txt", std::ofstream::out);
	if (writer.good()) {
		for (int i = 0; i < _countof(stuff); i++) {
			_itoa_s(stuff[i], tmp, 10);
			writer.write(tmp, strlen(tmp));
			writer.write("\n\0", strlen("\n\0"));
		}
		
	}
}

int Settings::getFPS() {
	return fps;
}

int Settings::getIconSize() {
	return iconSize;
}

bool Settings::isMenuOpen() {
	return openMenu;
}

// Controls the boolean 'openMenu',
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