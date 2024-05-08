#include "ConnectFour.hpp"
#include "Settings.hpp"
#include "Fonts.h"
#include "Menu.hpp"
#include "IncremOptMenu.hpp"


/*CONNECT FOUR
* 
* Project primarily done to get a better understanding of SFML
* 
* Made by Dylan Smith 5/6/24
* 
*/




/*TODO
* --now--
* find new drop sound
* controls visual
*  - for menu
*  - for gameplay + for sidebar
*  -- updates based on if in sidebar
* 
* --if revisiting this project--
* refactor settings menu bc its kinda a mess lowk
* maybe mouse functionality 
*/

int WinMain(void) {
	std::string gameTitle("Connect Four");

	Settings settings;
	auto menuWindow = sf::RenderWindow(sf::VideoMode(800, 600), gameTitle);

	sf::Font font;
	font.loadFromMemory(&Fonts::PixelifySans_ttf, Fonts::PixelifySans_ttf_len);

	Menu mainMenu(menuWindow, font, gameTitle, {
		"New Game",
		"Resume Game",
		"Settings",
		"Exit"
	});

	IncremOptMenu settingsMenu(menuWindow, font, "Settings", {
		"Board Width",
		"Board Height",
		"Win Requirement",
		"Player Count",
		"Reset Settings"
		}, {
			{ { 2, 32 }, &settings.mBoardWidth },
			{ { 2, 32 }, &settings.mBoardHeight },
			{ { 2, 32 }, &settings.mWinRequirement },
			{ { 2, 8 }, &settings.mPlayerCount }
	});

	ConnectFour c4(settings);

	std::thread gameThread;

	while (menuWindow.isOpen()) {
		switch (mainMenu.runMenu()) {
		case 0: //new game
			c4.resetBoard();
			c4.resetScores();
			[[fallthrough]];
		case 1: //resume game
			gameThread = c4.startGameInstance();
			menuWindow.close();
			gameThread.join();
			menuWindow.create(sf::VideoMode(800, 600), gameTitle);
			break;
		case 2: //settings
		{
			int out;
			while (true) {
				out = settingsMenu.runMenu();
				//back
				if (out == -1) break; 
				//reset
				else if (out == 4) {
					Settings def;
					settings = def;
				};
			}
		}
			settingsMenu.resetSelection();
			settings.adjustPlayersByEnteredCount();
			break;
		case 3: //exit
			menuWindow.close();
			return 0;
		default:
			break;
		}
	}
	return 1;
}
