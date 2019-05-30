#include "BoosterEnergyDrink.hpp"

EnergyDrink::EnergyDrink(ResourceManager & resMg, float scale)
{
	icon.setTexture(resMg.addTexture("sprites/items/inventory/energydrink.png"));
	icon.scale(scale, scale);
	usable = true;
	size = sf::Vector2u(1, 1);
}

void EnergyDrink::use()
{

}
