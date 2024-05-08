#pragma once
#ifndef CONNECT_FOUR_H
#define CONNECT_FOUR_H

#include "Fonts.h"
#include "Player.hpp"
#include "Settings.hpp"
#include "Sounds.h"
#include "UnitSizes.hpp"
#include "Token.hpp"
#include "Settings.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <thread>


class ConnectFour {
public:
	ConnectFour(Settings& settings);

	std::thread startGameInstance();
	
	void resetBoard(void);

	void resetScores(void);

private:
	void runGame(void);


	bool makeMove(int player, int column);

	//only checks for lines involving the most recent move to avoid redundant checks in unchanged parts of the board
	bool checkForWin(int lastX, int lastY) const;



	Settings& mSettings;

	int mMoveCount;

	std::vector<std::vector<Token>> mBoard;
	
};
#endif // !CONNECT_FOUR_H
