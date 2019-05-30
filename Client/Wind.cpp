#include "Wind.hpp"

Wind::Wind(ResourceManager & resMg)
	: ambient(resMg.addSound("sound/ambient/wind.ogg"))
{
	strengthNoise.SetFrequency(10.0);
	ambient.setLoop(true);
	ambient.setVolume(0);
	ambient.play();
}

Wind::~Wind()
{
	ambient.stop();
}

void Wind::update(sf::Time & time)
{
	const float t = static_cast<float>(time.asMicroseconds()) / 1000000.0f;
#ifdef WINDTEST
	strength = 1.0f;
	angle = t - std::floor(t / Math::TWO_PI)*Math::TWO_PI;
#else
	const float rawNoise = strengthNoise.GetValue(62151.0135692, 8213512.352156, t / 100.0f);
	progress = t + rawNoise;
	strength = std::abs(rawNoise);
	angle = Math::map(strengthNoise.GetValue(-12682.61838368, t / 100.0f, -337236.061521), -1, 1, 0, Math::TWO_PI);
#endif
	direction.x = std::sin(angle);
	direction.y = std::cos(angle);
	value = direction * strength;
	ambient.setVolume(Math::limit(strength*90.0f, 0.0f, 90.0f) + 10.0f);
}

const float & Wind::getAngle() const
{
	return this->angle;
}

const float & Wind::getStrength() const
{
	return this->strength;
}

const sf::Vector2f & Wind::getDirection() const
{
	return this->direction;
}

const sf::Vector2f & Wind::getValue() const
{
	return this->value;
}
