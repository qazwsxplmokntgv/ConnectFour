#include "IncremOptMenu.hpp"

void IncremOptMenu::loadAdditionalStaticElements()
{
	sf::ConvexShape upperArrow(3), lowerArrow(3);
	upperArrow.setPoint(0, sf::Vector2f(.85f * mWindow.getSize().x, .35f * mWindow.getSize().y));
	upperArrow.setPoint(1, sf::Vector2f(.88f * mWindow.getSize().x, .40f * mWindow.getSize().y));
	upperArrow.setPoint(2, sf::Vector2f(.82f * mWindow.getSize().x, .40f * mWindow.getSize().y));	
	lowerArrow.setPoint(0, sf::Vector2f(.85f * mWindow.getSize().x, .65f * mWindow.getSize().y));
	lowerArrow.setPoint(1, sf::Vector2f(.88f * mWindow.getSize().x, .60f * mWindow.getSize().y));
	lowerArrow.setPoint(2, sf::Vector2f(.82f * mWindow.getSize().x, .60f * mWindow.getSize().y));
	upperArrow.setFillColor(sf::Color::White);
	lowerArrow.setFillColor(sf::Color::White);
	mAdditionalStaticElements.push_back(std::make_shared<sf::ConvexShape>(upperArrow));
	mAdditionalStaticElements.push_back(std::make_shared<sf::ConvexShape>(lowerArrow));
}

void IncremOptMenu::loadAdditionalDynamicText()
{
	auto text = sf::Text("", mFont);
	text.setPosition(.85f * mWindow.getSize().x, .5f * mWindow.getSize().y);

	mAdditionalDynamicText.push_back(text);
}

void IncremOptMenu::inputUp()
{
	if (!mIncrementMode)
		Menu::inputUp();
	else {
		//increment value
		if (++(*mOptProperties[mCurrSelection].mOptValue) > mOptProperties[mCurrSelection].mOptLimits.second) {
			//wraps around if upper limit reached
			*mOptProperties[mCurrSelection].mOptValue = mOptProperties[mCurrSelection].mOptLimits.first;
		}
		mAdditionalDynamicText[0].setString(std::to_string(*(mOptProperties[mCurrSelection].mOptValue)));
		mAdditionalDynamicText[0].setOrigin(mAdditionalDynamicText[0].getLocalBounds().width / 2.f, mAdditionalDynamicText[0].getLocalBounds().height / 2.f);
	}
}

void IncremOptMenu::inputDown()
{
	if (!mIncrementMode)
		Menu::inputDown();
	else {
		//decrement value
		if (--(*mOptProperties[mCurrSelection].mOptValue) < mOptProperties[mCurrSelection].mOptLimits.first) {
			//wraps around if lower limit reached
			*mOptProperties[mCurrSelection].mOptValue = mOptProperties[mCurrSelection].mOptLimits.second;
		}
		mAdditionalDynamicText[0].setString(std::to_string(*(mOptProperties[mCurrSelection].mOptValue)));
		mAdditionalDynamicText[0].setOrigin(mAdditionalDynamicText[0].getLocalBounds().width / 2.f, mAdditionalDynamicText[0].getLocalBounds().height / 2.f);
	}
}

void IncremOptMenu::inputLeft()
{
	if (mIncrementMode)
		mIncrementMode = false;
	else
		Menu::inputLeft();
}

void IncremOptMenu::inputRight()
{
	if (!mIncrementMode) {
		mIncrementMode = true;
		mAdditionalDynamicText[0].setString(std::to_string(*(mOptProperties[mCurrSelection].mOptValue)));
		mAdditionalDynamicText[0].setOrigin(mAdditionalDynamicText[0].getLocalBounds().width / 2.f, mAdditionalDynamicText[0].getLocalBounds().height / 2.f);
	}
	else
		Menu::inputRight();
}
