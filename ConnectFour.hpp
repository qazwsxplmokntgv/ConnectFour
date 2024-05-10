#pragma once
#ifndef CONNECT_FOUR_H
#define CONNECT_FOUR_H

#include "Fonts.h"
#include "Player.hpp"
#include "Settings.hpp"
#include "Settings.hpp"
#include "Sounds.h"
#include "Token.hpp"
#include "UnitSizes.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <vector>

namespace UIColors {
	const auto main = sf::Color(100, 100, 100);
	const auto outline = sf::Color(0, 0, 0, 100);
	const auto selection = sf::Color(60, 60, 60);
}

class ConnectFour {
public:
	ConnectFour(Settings& settings);
	//ConnectFour(const ConnectFour& copy) = delete;
	//ConnectFour& operator=(const ConnectFour& copy) = delete;

	//false if force quit, true if otherwise exited
	bool runGame(void);

	void resetBoard(void);

	void resetScores(void);

	//redefines any elements whose appearance varies based on the size of the board
	void reloadSizeDependentElements();

private:
	//places a token owned by player in column
	//returns success of the move
	bool makeMove(int player, int column);

	//attempts to place a token based on mCurrPlayer and mSelectedCol, and updates various members accordingly
	void simulateMove(void);

	//performs the current sidebar selection action
	void executeSidebarSelection();

	//only checks for lines involving the most recent move to avoid redundant checks in unchanged parts of the board
	bool checkForWin(int lastX, int lastY) const;

	void inputLeft(void);
	void inputRight(void);
	void inputUp(void);
	void inputDown(void);

	int mMoveCount;

	int mCurrPlayer;
	int mSelectedCol;
	int mRoundWinner;

	bool mHoveringControlDiagram;

	bool mInSidebar;
	int mSideBarSelection;
	int mLastSideBarSelection;
	
	std::vector<std::vector<Token>> mBoard;

	//number of buttons in the sidebar
	constexpr static int sideBarButtonCount = 3;
	std::array<sf::RectangleShape, sideBarButtonCount> mSideBarBoxes;
	std::array<sf::Text, sideBarButtonCount> mSideBarTexts;



	Settings& mSettings;

	sf::RenderWindow mWindow;

	sf::RectangleShape mSelectionBar;

	sf::Font mFont;

	sf::SoundBuffer drop, win, sideBar;
	sf::Sound mDropSound, mWinSound, mSideBarSound;

	sf::RenderTexture scoreBoardTexture, controlDiagramTexture, gameControlTexture, sideBarControlTexture, boardBackgroundTexture;
	sf::Sprite mScoreBoard, mControlDiagram, mGameControlOverlay, mSideBarControlOverlay, mBoardBackground;
	
	sf::ConvexShape mControlDiagramArrow;
};
#endif // !CONNECT_FOUR_H
