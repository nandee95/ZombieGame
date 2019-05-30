#pragma once
#include "Particle.hpp"
#include <vector>
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>
#include "../System/Math.hpp"
#include "../System/ResourceManager.hpp"
#include "../Entities/Particle.hpp"
#include "../Entities/Entity.hpp"

class ParticleEmitter : public client::Entity
{
private:
	sf::Vector2f position;
protected:
	std::vector<std::shared_ptr<Particle>> particles;
public:
	ParticleEmitter();

	void reset();

	const void setPosition(sf::Vector2f position);
	const sf::Vector2f getPosition() const;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void emitParticle(const sf::Time& time) = 0;

};

class SmokeParticleEmitter : public ParticleEmitter
{
private:
	const float delay = 25000.0f;
	ResourceManager& resMg;
	sf::Time lastEmit;
public:
	SmokeParticleEmitter(sf::Vector2f position, ResourceManager& resMg);

	virtual void emitParticle(const sf::Time& time);

	virtual void update(const sf::Time& time, const sf::Time& dt, Wind& wind);
};