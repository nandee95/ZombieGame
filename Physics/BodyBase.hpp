#pragma once

#include <SFML/Graphics.hpp>

enum BodyType : sf::Int8
{
	StaticBody,
	DynamicBody
};

class Physics;

class Body
{
private:
	//sf::Int8 layer = 0;
	//std::set<sf::Int8> collidesWith;
	BodyType type = BodyType::StaticBody;
	sf::Vector2f position;
	sf::Vector2f velocity;
protected:
	friend Physics;
	sf::Vector2f _nextPosition;
public:

	virtual ~Body();

	const sf::Vector2f getPosition() const;
	void setPosition(sf::Vector2f position);
	const void setType(BodyType type);
	const BodyType& getType() const;
	const void setVelocity(sf::Vector2f velocity);

	const sf::Vector2f getVelocity() const;
};