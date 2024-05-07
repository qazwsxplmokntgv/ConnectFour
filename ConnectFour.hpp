#pragma once
#ifndef CONNECT_FOUR_H
#define CONNECT_FOUR_H

#include <vector>
#include "Player.hpp"
#include "Token.hpp"
#include "Settings.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

class ConnectFour {
public:
	ConnectFour(const Settings& settings);

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
