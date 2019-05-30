#include "RectBody.hpp"

RectBody::RectBody(sf::Vector2f size) : size(size)
{

}

const sf::Vector2f RectBody::getSize() const
{
	return this->size;
}
