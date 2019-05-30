#pragma once

#include "BodyBase.hpp"

#include <SFML/Graphics.hpp>

class RectBody : public Body
{
private:
	sf::Vector2f size;
public:
	RectBody(sf::Vector2f size);
	const sf::Vector2f getSize() const;
};
