#pragma once
#ifndef CONNECT_FOUR_H
#define CONNECT_FOUR_H

#include <vector>
#include "Player.hpp"
#include "Token.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

class ConnectFour {
public:
	ConnectFour(int boardHeight = 6, int boardWidth = 7, int winRequirement = 4, 
		std::vector<Player> players = { Player(Token(0, sf::Color::Red)), Player(Token(1, sf::Color::Yellow)) });
	void runGame(void);
private:
	bool makeMove(int player, int column);

	//only checks for lines involving the most recent move to avoid redundant checks in unchanged parts of the board
	bool checkForWin(int lastX, int lastY) const;


	int mBoardHeight;
	int mBoardWidth;
	int mWinRequirement;

	std::vector<std::vector<Token>> mBoard;
	
	std::vector<Player> mPlayers;
};
#endif // !CONNECT_FOUR_H
