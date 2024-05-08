#pragma once
#ifndef INCREM_OPT_MENU_H
#define INCREM_OPT_MENU_H

#include "Menu.hpp"
#include <utility>

struct IncremOptProperties {
	std::pair<int, int> mOptLimits;
	int* mOptValue;
};

//menus consisting of options each associated with an integer value 
//the user can increment or decrement these values during menu runtime
class IncremOptMenu : public Menu {
public:
	IncremOptMenu(sf::RenderWindow& window, sf::Font& font, std::string title, std::vector<std::string> opts, std::vector<IncremOptProperties> optProperties) :
		Menu(window, font, title, opts, true, &this->mIncrementMode),
		mOptProperties(optProperties),
		mIncrementMode(false) {}

	void loadAdditionalStaticElements();
	void loadAdditionalDynamicText();

private:
	void inputUp();
	void inputDown();
	void inputLeft();
	void inputRight();

	//highest and lowest valid value for each option, and a ptr to its current value
	//MUST INCLUDE PROPERTIES FOR EACH OPT //may be changed eventually to support opts defaulting to base behavior
	std::vector<IncremOptProperties> mOptProperties;

	//whether the menu should run as a normal menu or not
	bool mIncrementMode;
};

#endif // !INC_OPT_MENU_H
