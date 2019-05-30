#include "ItemBase.hpp"

Item::Item()
{

}

const sf::Int8 & Item::getLevel() const
{
	return this->level;
}

const bool & Item::isUsable() const
{
	return usable;
}

const sf::Vector2u & Item::getSize() const
{
	return this->size;
}

const sf::Sprite & Item::getIcon() const
{
	return this->icon;
}

void Item::use() {}
