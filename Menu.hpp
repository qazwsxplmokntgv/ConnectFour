#pragma once
#ifndef MENU_H
#define MENU_H

#include "Settings.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Menu {
public:
	Menu(std::string title, std::vector<std::string> opts) :
		mTitle(title),
		mMenuOptions(opts),
		mCurrSelection(0) {}
	int runMenu(sf::RenderWindow& window);
private:
	std::string mTitle;
	std::vector<std::string> mMenuOptions;
	int mCurrSelection;
};

#endif // !MENU_H
