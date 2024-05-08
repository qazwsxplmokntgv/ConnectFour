#pragma once
#ifndef MENU_H
#define MENU_H

#include "Settings.hpp"
#include "Sounds.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <utility>
#include <vector>

class Menu {
public:
	Menu(sf::RenderWindow& window, sf::Font& font, std::string title, std::vector<std::string> opts, bool isSubMenu = false, bool* shouldIncludeAdditionalElements = nullptr);
	
	int runMenu();

	void resetSelection(void);

protected:
	virtual void inputDown(void);
	virtual void inputUp(void);
	virtual void inputLeft(void);
	virtual void inputRight(void);

	sf::Text mTitle;
	std::vector<std::string> mMenuOptions;

	sf::Font& mFont;

	sf::Sound mKeypressSound;

	int mCurrSelection;

	bool mIsSubMenu;

	bool mPersistInMenu;

	sf::RenderWindow& mWindow;

	//used as placeholders here, for use in derived classes
	std::vector<std::shared_ptr<sf::Drawable>> mAdditionalStaticElements;
	std::vector<sf::Text> mAdditionalDynamicText;
	bool* mShouldIncludeAdditionalElements;
	virtual void loadAdditionalStaticElements(void) {}
	virtual void loadAdditionalDynamicText(void) {}
};

#endif // !MENU_H
