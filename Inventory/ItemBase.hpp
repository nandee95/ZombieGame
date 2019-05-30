#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include "../System/ResourceManager.hpp"

class Item
{
protected:
	bool usable = false;
	sf::Int8 level;
	sf::Sprite icon;
	sf::Vector2u size;
public:
	Item();

	const sf::Int8& getLevel() const;

	const bool& isUsable() const;
	const sf::Vector2u& getSize() const;

	const sf::Sprite& getIcon() const;

	virtual void use();
};