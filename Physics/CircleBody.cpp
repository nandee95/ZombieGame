#include "CircleBody.hpp"

CircleBody::CircleBody(float radius) : radius(radius)
{

}

const float CircleBody::getRadius() const
{
	return this->radius;
}
