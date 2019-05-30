#include "Entity.hpp"

void client::Entity::update(const sf::Time & time, const sf::Time & dt, Wind & wind)
{

}

void client::Entity::drawPre(sf::RenderTarget & target) const
{

}

const sf::FloatRect & client::Entity::getAABB()
{
	return this->AABB;
}
