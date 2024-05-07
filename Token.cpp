#include "Token.hpp"

Token::Token(int playerID, sf::Color tokenColor)
{
	mPlayerID = playerID;
	mTokenGraphic = sf::CircleShape(tokenProp::radius);
	mTokenGraphic.setFillColor(tokenColor);
	mTokenGraphic.setOutlineThickness(tokenProp::outlineThickness);
	mTokenGraphic.setOutlineColor(sf::Color(50, 50, 50));
}

const sf::CircleShape& Token::getTokenGraphic(void) const
{
	return mTokenGraphic;
}

const int Token::getID(void) const
{
	return mPlayerID;
}

sf::Color Token::getColor(void) const
{
	return mTokenGraphic.getFillColor();
}

void Token::setPosition(float x, float y)
{
	mTokenGraphic.setPosition(x, y);
}
