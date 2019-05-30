#include "ParticleEmitter.hpp"

ParticleEmitter::ParticleEmitter()
{

}

void ParticleEmitter::reset()
{
	this->particles.empty();
}

const void ParticleEmitter::setPosition(sf::Vector2f position)
{
	this->position = position;
}

const sf::Vector2f ParticleEmitter::getPosition() const
{
	return this->position;
}

void ParticleEmitter::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	for (auto& p : particles)
	{
		target.draw(*p);
	}
}

SmokeParticleEmitter::SmokeParticleEmitter(sf::Vector2f position, ResourceManager & resMg) : resMg(resMg)
{
	this->setPosition(Math::worldToScreen(position));
	this->screen_y = getPosition().y;
}

void SmokeParticleEmitter::emitParticle(const sf::Time & time)
{
	this->particles.push_back(std::make_shared<SmokeParticle>(resMg, getPosition(), time));
}

void SmokeParticleEmitter::update(const sf::Time & time, const sf::Time & dt, Wind & wind)
{
	if (time >= lastEmit + sf::microseconds(delay))
	{
		const int times = static_cast<int>(std::ceil(static_cast<float>((time - lastEmit).asMicroseconds()) / delay));
		for (int i = 0; (i < times) && particles.size() < 500; i++)
			this->emitParticle(time);
		lastEmit = time;
	}
	for (auto it = particles.begin(); it != particles.end(); )
	{
		if ((*it)->killMe)
		{
			it = particles.erase(it);
		}
		else
		{
			(*it)->update(time, dt);
			it++;
		}
	}
}
