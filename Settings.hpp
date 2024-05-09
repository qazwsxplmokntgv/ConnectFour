#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include "Player.hpp"
#include <array>
#include <vector>
#include <vector>

const static std::array<sf::Color, 8> colors = {
	sf::Color::Red,
	sf::Color::Yellow,
	sf::Color::Blue,
	sf::Color::Green,
	sf::Color::Magenta,
	sf::Color::Cyan,
	sf::Color(225,100,0), //orange
	sf::Color::White
};

struct Settings {
	int mBoardWidth = 7;
	int mBoardHeight = 6;
	int mWinRequirement = 4;
	int mPlayerCount = 2;
	std::vector<Player> mPlayerInfo = { Player(Token(0, colors[0])), Player(Token(1, colors[1]))};

	//adds or removes players to match the playerCount value;
	void adjustPlayersByEnteredCount();

private:
	void addPlayer(void);
	void removePlayer(void);
};

#endif // !SETTINGS_H
