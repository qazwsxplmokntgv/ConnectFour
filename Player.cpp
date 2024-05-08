#include "Player.hpp"

const Token& Player::getToken(void) const
{
	return mToken;
}

int Player::getWinCount(void) const
{
	return mWinCount;
}

void Player::incWinCount(void)
{
	++mWinCount;
}

void Player::resetWinCount(void)
{
	mWinCount = 0;
}
