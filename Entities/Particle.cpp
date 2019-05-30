#include "Particle.hpp"
#include "ParticleEmitter.hpp"

SmokeParticle::SmokeParticle(ResourceManager & resMg, sf::Vector2f position, sf::Time created) : created(created)
{
	sf::Texture& tex = resMg.addTexture("sprites/particle/smoke.png");
	sprite.setTexture(tex);
	sprite.setOrigin(sf::Vector2f(tex.getSize()) / 2.0f);

	float strength = Math::random(32.f, 96.f);
	sprite.setColor(sf::Color(strength, strength, strength));

	sprite.setPosition(position + sf::Vector2f(Math::random(-100.0f, 100.0f), 0));

	float scale = Math::random(0.5f, 1.0f);
	sprite.setScale(scale, scale);

	sprite.setRotation(Math::random(0.f, 360.0f));
	this->lifeTime = sf::milliseconds(Math::random(500.0f, 1000.f));
	angle = Math::random(3.14 - 0.5f, 3.14 + 0.5f);//TODO
	this->velocity = sf::Vector2f(std::sin(angle), std::cos(angle))* Math::random(30.0f, 250.0f);
	stateChange = created;
}

void SmokeParticle::update(const sf::Time & time, const sf::Time & dt)
{
	sf::Time delta = time - stateChange;
	switch (state)
	{
	case FadeIn:
	{
		if (fadeInDelay <= delta)
		{
			this->state = Stay;
			stateChange = time;
			const sf::Color& color = this->sprite.getColor();
			this->sprite.setColor(sf::Color(color.r, color.g, color.b, 255.0f));
			break;
		}
		const sf::Color& color = this->sprite.getColor();
		this->sprite.setColor(sf::Color(color.r, color.g, color.b, static_cast<float>(delta.asMicroseconds()) / static_cast<float>(fadeOutDelay.asMicroseconds())*255.0f));
	} break;
	case Stay:
	{
		if (lifeTime <= delta)
		{
			this->state = FadeOut;
			stateChange = time;
			break;
		}
	} break;
	case FadeOut:
	{
		if (fadeOutDelay <= delta)
		{
			this->killMe = true;
			return;
		}

		const sf::Color& color = this->sprite.getColor();
		this->sprite.setColor(sf::Color(color.r, color.g, color.b, (1.0f - static_cast<float>(delta.asMicroseconds()) / static_cast<float>(fadeOutDelay.asMicroseconds()))*255.0f));
	} break;
	}
	sprite.move(velocity*(static_cast<float>(dt.asMicroseconds()) / 1000000.0f));
}

void SmokeParticle::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(this->sprite);
}
