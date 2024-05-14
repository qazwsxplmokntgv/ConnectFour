#include "ConnectFour.hpp"
#include "Fonts.h"
#include "IncremOptMenu.hpp"
#include "Menu.hpp"
#include "Settings.hpp"


/*CONNECT FOUR
* 
* Personal Project to develop SFML skills.
* Customizability provided through a number of settings as a proof of concept.
* Based on the well-known game of the same name.
* 
* Instructions:
*	Take turns dropping tokens into any column of the player's choice.
*	The first player to land 4 tokens in a contiguous line, vertically, horizontally, or diagonally, wins.
* 
* Made by Dylan Smith 5/6/24
* 
*/




/*TODO
* --now--
* 
* --if revisiting this project--
* just redo the menu classes entirely
* -fix menu sounds not playing when going between menus
* -refactor settings menu bc its not the cleanest
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

	while (menuWindow.isOpen()) {
		switch (mainMenu.runMenu()) {
		case 0: //new game
			c4.resetBoard();
			c4.resetScores();
			[[fallthrough]];
		case 1: //resume game
			menuWindow.close();
			if (c4.runGame()) 
				//reopen menu if c4 was left via main menu button
				menuWindow.create(sf::VideoMode(800, 600), gameTitle);
			break;
		case 2: { //settings
			int out;
			bool stayInSettings = true;
			while (stayInSettings) {
				out = settingsMenu.runMenu();
				switch (out) {
				case 0:
				case 1:
				case 2:
				case 3:
					break;
				case 4: {
					Settings def;
					settings = def;
					break;
				}
				default:
					stayInSettings = false;
					break;
				}
			}
		}
			settingsMenu.resetSelection();
			settings.adjustPlayersByEnteredCount();
			break;
		case 3: //exit
			menuWindow.close();
			return 0;
		}
	}
	return 1;
}
