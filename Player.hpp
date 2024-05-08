#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "Token.hpp"

class Player {
public:
	Player(Token token) :
		mToken(token),
		mWinCount(0) {}

	const Token& getToken(void) const;
	int getWinCount(void) const;
	void incWinCount(void);
	void resetWinCount(void);

private:
	Token mToken;
	int mWinCount;
};

#endif // !PLAYER_H
