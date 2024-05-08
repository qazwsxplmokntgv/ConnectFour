#include "Settings.hpp"

void Settings::adjustPlayersByEnteredCount()
{
	while (mPlayerInfo.size() < mPlayerCount) addPlayer();
	while (mPlayerInfo.size() > mPlayerCount) removePlayer();
}

void Settings::addPlayer(void)
{
	if (mPlayerInfo.size() < 8)
		mPlayerInfo.push_back(Player(Token((int)mPlayerInfo.size(), colors[(int)mPlayerInfo.size()])));
}

void Settings::removePlayer(void)
{
	if (mPlayerInfo.size() > 0)
		mPlayerInfo.pop_back();
}
