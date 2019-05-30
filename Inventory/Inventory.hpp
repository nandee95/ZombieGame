#pragma once
#include "ItemBase.hpp"
#include "BoosterEnergyDrink.hpp"
#include <vector>
#include <memory>
#include <SFML/Graphics/Drawable.hpp>
#include "../System/ResourceManager.hpp"

enum ItemType
{
	ItemTool,
	ItemBooster
};

struct ItemHash {
	size_t operator()(const sf::Vector2u& t) const {
		return t.x * 46340 + t.y;
	}
};


class InventorySlot : public sf::Drawable
{
private:
	sf::Sprite slotBg;
	sf::Vector2u slotSize;
	float itemSize;
	std::unordered_map<sf::Vector2u, std::shared_ptr<Item>, ItemHash> items;
public:
	InventorySlot(sf::Vector2u slotSize, sf::Vector2f localPos, float itemScale, float itemSize, ResourceManager& resMg);

	const sf::Vector2f& getLocalPosition() const;

	void removeItem(std::shared_ptr<Item>& item);

	const sf::Vector2f getSize() const;

	const sf::Vector2u& getSlotSize() const;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	const bool slotTaken(sf::Vector2u slot);

	const bool itemFitsToSlot(sf::Vector2u slot, sf::Vector2u size);

	std::pair<std::shared_ptr<Item>, sf::Vector2f> getItemAtPosition(sf::Vector2f position);

	sf::Vector2u getSlotAtPosition(sf::Vector2f position);

	const bool pushItem(std::shared_ptr<Item> item);

	void addItemToPosition(std::shared_ptr<Item> item, sf::Vector2u pos);
};

class Inventory : public sf::Drawable, public sf::Transformable
{
protected:
	std::vector<InventorySlot> slots;
	sf::RectangleShape background;
	sf::Text title;
	float itemSize = 128;
	std::shared_ptr<Item> draggedItem;
	sf::Vector2f dragPosition;
	sf::Vector2f dragShift;
public:
	Inventory(ResourceManager& resMg);

	/*
	const std::map<sf::Vector2u, std::shared_ptr<Item>,ItemHash>&  raw() const
	{
		return items;
	}*/

	const bool pushItem(std::shared_ptr<Item> item);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void event(sf::Event& e);
};

class DefaultInventory : public Inventory
{
private:
	const sf::Vector2f size;
	float itemScale;
public:
	DefaultInventory(ResourceManager& resMg, sf::Vector2u resolution);
};