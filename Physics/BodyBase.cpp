#include "BodyBase.hpp"

#include "../Physics/Physics.hpp"
Body::~Body() {}

const sf::Vector2f Body::getPosition() const
{
	return this->position;
}

void Body::setPosition(sf::Vector2f position)
{
	this->position = position;
	if (type == StaticBody)
		this->_nextPosition = position;
}

const void Body::setType(BodyType type)
{
	this->type = type;
	if (type == StaticBody)
		this->_nextPosition = position;
}

const BodyType & Body::getType() const
{
	return this->type;
}

const void Body::setVelocity(sf::Vector2f velocity)
{
	this->velocity = velocity;
}

const sf::Vector2f Body::getVelocity() const
{
	return this->velocity;
}
