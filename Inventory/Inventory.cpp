#include "Inventory.hpp"

InventorySlot::InventorySlot(sf::Vector2u slotSize, sf::Vector2f localPos, float itemScale, float itemSize, ResourceManager & resMg) : slotSize(slotSize), itemSize(itemSize),
slotBg(resMg.addTexture("sprites/items/inventoryslot/slot.png", ResourceManager::TextureRepeat))
{
	slotBg.setScale(itemScale, itemScale);
	slotBg.setPosition(localPos);
	slotBg.setTextureRect(sf::IntRect(sf::Vector2i(), sf::Vector2i(128 * slotSize.x, 128 * slotSize.y)));
}

const sf::Vector2f & InventorySlot::getLocalPosition() const
{
	return slotBg.getPosition();
}

void InventorySlot::removeItem(std::shared_ptr<Item>& item)
{
	for (auto it = items.begin(); it != items.end(); it++)
	{
		if (it->second == item)
		{
			items.erase(it);
			break;
		}
	}
}

const sf::Vector2f InventorySlot::getSize() const
{
	return sf::Vector2f(slotSize)*itemSize;
}

const sf::Vector2u & InventorySlot::getSlotSize() const
{
	return slotSize;
}

void InventorySlot::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(slotBg, states);

	for (auto& i : items)
	{
		sf::RenderStates copyStates = states;
		copyStates.transform.translate(sf::Vector2f(i.first)*itemSize);
		target.draw(i.second->getIcon(), copyStates);
	}
}

const bool InventorySlot::slotTaken(sf::Vector2u slot)
{
	return (items.count(slot) == 1);
}

const bool InventorySlot::itemFitsToSlot(sf::Vector2u slot, sf::Vector2u size)
{
	bool good = true;
	for (int yy = 0; yy < size.y; yy++)
	{
		if (!good) break;
		for (int xx = 0; xx < size.x; xx++)
		{
			if (slotTaken(sf::Vector2u(slot.x + xx, slot.y + yy)))
			{
				good = false;
				break;
			}
		}
	}
	return good;
}

std::pair<std::shared_ptr<Item>, sf::Vector2f> InventorySlot::getItemAtPosition(sf::Vector2f position)
{
	const sf::Vector2f rpos(position / itemSize);
	const sf::Vector2u pos(std::floor(rpos.x), std::floor(rpos.y));
	if (!slotTaken(pos)) return std::make_pair(nullptr, sf::Vector2f());

	return std::make_pair(items.at(pos), rpos - sf::Vector2f(pos));
}

sf::Vector2u InventorySlot::getSlotAtPosition(sf::Vector2f position)
{
	const sf::Vector2f rpos(position / itemSize);
	return sf::Vector2u(std::floor(rpos.x), std::floor(rpos.y));
}

const bool InventorySlot::pushItem(std::shared_ptr<Item> item)
{
	sf::Vector2u slot;
	bool found = false;
	const sf::Vector2u& slots = getSlotSize();
	for (unsigned int y = 0; y < slots.y - item->getSize().y + 1; y++)
	{
		for (unsigned int x = 0; x < slots.x - item->getSize().x + 1; x++)
		{
			if (itemFitsToSlot(sf::Vector2u(x, y), item->getSize()))
			{
				slot = sf::Vector2u(x, y);
				goto insertItem;
			}
		}
	}

	return false;
insertItem:
	this->items.insert(std::make_pair(slot, item));
	return true;
}

void InventorySlot::addItemToPosition(std::shared_ptr<Item> item, sf::Vector2u pos)
{
	items.insert(std::make_pair(pos, item));
}

Inventory::Inventory(ResourceManager & resMg)
{
	title.setFont(resMg.addFont("fonts/arial.ttf"));
	title.setFillColor(sf::Color(255, 255, 255, 127));
	background.setFillColor(sf::Color(0, 0, 0, 127));
	background.setOutlineThickness(1.0f);
	background.setOutlineColor(sf::Color::Black);
}

const bool Inventory::pushItem(std::shared_ptr<Item> item)
{
	for (auto& s : this->slots)
	{
		if (s.pushItem(item)) return true;
	}
	return false;
}

void Inventory::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.transform = Transformable::getTransform();
	target.draw(background, states);
	states.transform.translate(5, 5);
	target.draw(title, states);
	states.transform.translate(0, title.getGlobalBounds().height + 10);
	for (auto& s : slots)
	{
		target.draw(s, states);
	}

	if (draggedItem != nullptr)
	{
		sf::RenderStates s;
		s.transform.translate(dragPosition + dragShift);
		target.draw(draggedItem->getIcon(), s);
	}
}

void Inventory::event(sf::Event & e)
{
	switch (e.type)
	{
	case sf::Event::MouseButtonPressed:
	{
		if (e.mouseButton.button == sf::Mouse::Left)
		{
			const sf::Vector2f bgPos = getPosition() + sf::Vector2f(5, title.getGlobalBounds().height + 15);
			for (auto& s : slots)
			{
				if (sf::FloatRect(bgPos + s.getLocalPosition(), s.getSize()).contains(e.mouseButton.x, e.mouseButton.y))
				{
					auto item = s.getItemAtPosition(sf::Vector2f(e.mouseButton.x, e.mouseButton.y) - bgPos - s.getLocalPosition());
					if (item.first == nullptr) return;
					draggedItem = item.first;
					dragShift = item.second* -itemSize;
					std::cout << item.second.x << " " << item.second.y << "\n";
					dragPosition = sf::Vector2f(e.mouseButton.x, e.mouseButton.y);
					std::cout << "Dragged" << std::endl;
				}
			}
		}
	} break;
	case sf::Event::MouseMoved:
	{
		if (draggedItem != nullptr)
			dragPosition = sf::Vector2f(e.mouseMove.x, e.mouseMove.y);
	} break;
	case sf::Event::MouseButtonReleased:
	{
		if (draggedItem != nullptr)
		{
			const sf::Vector2f bgPos = getPosition() + sf::Vector2f(5, title.getGlobalBounds().height + 15);
			for (auto& s : slots)
			{
				if (sf::FloatRect(bgPos + s.getLocalPosition(), s.getSize()).contains(e.mouseButton.x, e.mouseButton.y))
				{
					sf::Vector2u slot = s.getSlotAtPosition(sf::Vector2f(e.mouseButton.x, e.mouseButton.y) - bgPos - s.getLocalPosition());
					if (s.slotTaken(slot)) break;

					s.removeItem(draggedItem);
					s.addItemToPosition(draggedItem, slot);
				}
			}

			draggedItem = nullptr;
		} break;
	}
	}
}

DefaultInventory::DefaultInventory(ResourceManager & resMg, sf::Vector2u resolution) :
	Inventory(resMg)
{
	this->Inventory::setPosition(resolution.x*0.25f, 100.0f);
	this->title.setString("Character inventory");

	this->itemSize = ((resolution.x*0.5f - 10.0f) / 10.0f);
	this->itemScale = itemSize / 128.0f;
	this->slots.push_back(InventorySlot(sf::Vector2u(10, 2), sf::Vector2f(), itemScale, itemSize, resMg));
	this->background.setSize(sf::Vector2f(resolution.x*0.5f, title.getLocalBounds().height + 20 + slots.at(0).getSize().y));

	if (!this->pushItem(std::make_shared<EnergyDrink>(resMg, itemScale))) std::cout << "Falied to insert item";
	if (!this->pushItem(std::make_shared<EnergyDrink>(resMg, itemScale))) std::cout << "Falied to insert item";
	if (!this->pushItem(std::make_shared<EnergyDrink>(resMg, itemScale))) std::cout << "Falied to insert item";
}
