#pragma once
#include "noise/noise.h"
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Audio/Sound.hpp>
#include "../System/ResourceManager.hpp"
#include "../System/Math.hpp"
#include <iostream>

#include <random>

//#define WINDTEST

class Wind
{
private:
	//Noises
	noise::module::Perlin strengthNoise;//Time based

	//Stored values
	float progress = 0.0f;
	float angle = 0.0f;
	float strength=0.0f;
	sf::Vector2f direction;
	sf::Vector2f value;

	//Resources
	sf::Sound ambient;
	std::default_random_engine generator;
public:
	Wind(ResourceManager& resMg);

	~Wind();

	void update(sf::Time& time);
	const float& getAngle() const;
	const float& getStrength() const;
	const sf::Vector2f& getDirection() const;
	const sf::Vector2f& getValue() const;
};