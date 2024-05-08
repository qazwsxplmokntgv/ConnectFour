#pragma once
#ifndef CONNECT_FOUR_H
#define CONNECT_FOUR_H

#include "Fonts.hpp"
#include "Player.hpp"
#include "Settings.hpp"
#include "Sounds.hpp"
#include "Token.hpp"
#include "Settings.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <vector>

class ConnectFour {
public:
	ConnectFour(Settings& settings);

	void runGame(void);
private:
	bool makeMove(int player, int column);

	//only checks for lines involving the most recent move to avoid redundant checks in unchanged parts of the board
	bool checkForWin(int lastX, int lastY) const;

	void resetBoard(void);

	int mBoardHeight;
	int mBoardWidth;
	int mWinRequirement;

	std::vector<std::vector<Token>> mBoard;
	
	std::vector<Player>* mPlayers;
};
#endif // !CONNECT_FOUR_H
