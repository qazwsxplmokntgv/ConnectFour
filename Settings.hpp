#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include "Player.hpp"
#include <vector>
#include <array>

const static int absolute_player_limit = 8;

const static std::array<sf::Color, absolute_player_limit> colors = {
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
	std::vector<Player> mPlayerInfo = { Player(Token(0, colors[0])), Player(Token(1, colors[1]))};

	void addPlayer(void);
	void removePlayer(void);
};

#endif // !SETTINGS_H
