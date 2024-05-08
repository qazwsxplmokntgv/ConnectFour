#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include <SFML/Graphics.hpp>

namespace tokenProp {
	const float radius = 50.f;
	const float outlineThickness = -10.f;
}

class Token {
public:
	Token(int playerID, sf::Color tokenColor);

	const sf::CircleShape& getTokenGraphic(void) const;
	const int getID(void) const;
	sf::Color getColor(void) const;

	void setPosition(float x, float y);

	bool operator==(const Token& rhs) const;
private:
	int mPlayerID;
	sf::CircleShape mTokenGraphic;
};

#endif // !TOKEN_H
