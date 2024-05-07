#include "ConnectFour.hpp"
#include "Settings.hpp"
#include "Menu.hpp"


/*TODO
* spruce up menu
* settings changeability & associated ui stuff
* find a clean way to pause for the win message reliably
* find new drop sound
* distinction between hard and soft game close
* maybe mouse functionality 
*/

int main(void) {
	Settings settings;
	sf::RenderWindow menuWindow(sf::VideoMode(800, 600), "Connect Four");


	Menu mainMenu("Connect Four", {
		"Play",
		"Settings",
		"Exit"
	});

	Menu settingsMenu("Settings", {
		"Board Width",
		"Board Height",
		"Win Sequence Requirement",
		"Player Count"
		"Back"
	});

	ConnectFour c4(settings);

	while (true) {
		switch (mainMenu.runMenu(menuWindow)) {
		case 0: //play
			menuWindow.close();
			c4.runGame();
			return 0;
		case 1: //settings
			switch (settingsMenu.runMenu(menuWindow)) {
			case 0: //width
				break;
			case 1: //height
				break;
			case 2: //win req
				break;
			case 3: //player count
				break;
			default: //back
				continue;
			}
			break;
		default: //exit
			if (menuWindow.isOpen()) menuWindow.close();
			return 0;
		}
	}
}
