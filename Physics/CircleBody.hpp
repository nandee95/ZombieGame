#pragma once
#include "BodyBase.hpp"
#include <SFML/Graphics.hpp>


class CircleBody : public Body
{
private:
	float radius;
public:
	CircleBody(float radius);

	const float getRadius() const;
};