#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "../System/Math.hpp"
#include "../System/ResourceManager.hpp"

class Particle : public sf::Drawable
{
protected:
	sf::Vector2f velocity;
public:
	bool killMe = false;
	virtual void update(const sf::Time& time, const sf::Time& dt) = 0;
};



class SmokeParticle : public Particle
{
private:
	enum SmokeParticleState
	{
		FadeIn,
		Stay,
		FadeOut
	};
	const sf::Time fadeOutDelay = sf::milliseconds(500);
	const sf::Time fadeInDelay = sf::milliseconds(500);
	sf::Sprite sprite;
	sf::Time lifeTime;
	sf::Time created;
	sf::Time stateChange;
	bool fadeOut = false;
	float angle = 0.0f;
	SmokeParticleState state = FadeIn;
public:
	SmokeParticle(ResourceManager& resMg, sf::Vector2f position, sf::Time created);
	virtual void update(const sf::Time& time, const sf::Time& dt);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};