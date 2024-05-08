#include "ConnectFour.hpp"
#include "Settings.hpp"
#include "Fonts.hpp"
#include "Menu.hpp"
#include "IncremOptMenu.hpp"


/*TODO
* refactor settings menu bc its kinda a mess lowk
* spruce up menu
* find a clean way to pause for the win message reliably
* find new drop sound
* maybe customizable keybinds
* maybe distinction between hard and soft game close
* maybe mouse functionality 
*/

int WinMain(void) {
	std::string gameTitle("Connect Four");

	Settings settings;
	auto menuWindow = sf::RenderWindow(sf::VideoMode(800, 600), gameTitle);

	sf::Font font;
	font.loadFromMemory(&Fonts::PixelifySans_ttf, Fonts::PixelifySans_ttf_len);

	Menu mainMenu(menuWindow, font, gameTitle, {
		"Play",
		//"Settings", /*so many problems. i will fix later*/
		"Exit"
	});

	IncremOptMenu settingsMenu(menuWindow, font, "Settings", {
		"Board Width",
		"Board Height",
		"Win Requirement",
		"Player Count"
		}, {
			{ { 1, 64 }, &settings.mBoardWidth },
			{ { 1, 64 }, &settings.mBoardHeight },
			{ { 1, 64 }, &settings.mWinRequirement },
			{ { 1, 64 }, &settings.mPlayerCount }
	});

	ConnectFour c4(settings);

	while (menuWindow.isOpen()) {
		switch (mainMenu.runMenu()) {
		case 0: //play
			menuWindow.close();
			c4.runGame();
			menuWindow.create(sf::VideoMode(800, 600), gameTitle);
			break;
		case 1: //settings
			//while (settingsMenu.runMenu() != -1);
			//settingsMenu.resetSelection();
			//settings.adjustPlayersByEnteredCount();
			//break;
		case 2: //exit
			menuWindow.close();
			return 0;
		default:
			break;
		}
	}
	return 1;
}
