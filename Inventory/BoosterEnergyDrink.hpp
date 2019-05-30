#pragma once

#include "ItemBase.hpp"

class EnergyDrink : public Item
{
private:

public:
	EnergyDrink(ResourceManager& resMg, float scale);

	virtual void use();
};