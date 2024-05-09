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


class ConnectFour {
public:
	ConnectFour(Settings& settings);

	//false if force quit, true if otherwise exited
	bool runGame(void);

	void resetBoard(void);

	void resetScores(void);

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

	
	int mMoveCount;

	int mCurrPlayer;
	int mSelectedCol;
	int mRoundWinner;

	bool mInSidebar;
	int mSideBarSelection;
	int mLastSideBarSelection;
	
	std::vector<std::vector<Token>> mBoard;

	//number of buttons in the sidebar
	constexpr static int sideBarButtonCount = 3;
	std::array<sf::RectangleShape, sideBarButtonCount> mSideBarBoxes;
	std::array<sf::Text, sideBarButtonCount> mSideBarTexts;

	//number of groups of bounding boxes
	constexpr static int boundingRegionCount = 2;
	std::array<std::vector<sf::FloatRect>, boundingRegionCount> mBoundingRegions;

	Settings& mSettings;

	sf::RenderWindow mWindow;

	sf::RectangleShape mSelectionBar;

	sf::Font mFont;

	sf::SoundBuffer drop, win, sideBar;
	sf::Sound mDropSound, mWinSound, mSideBarSound;

	sf::RenderTexture scoreBoardTexture, controlDiagramTexture, gameControlTexture, sideBarControlTexture, boardBackgroundTexture;
	sf::Sprite mScoreBoard, mControlDiagram, mGameControlOverlay, mSideBarControlOverlay, mBoardBackground;
	
};
#endif // !CONNECT_FOUR_H
