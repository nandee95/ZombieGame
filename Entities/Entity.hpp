#pragma once
#include <SFML/Graphics.hpp>
#include "../Client/Wind.hpp"

namespace client
{
	class Entity : public sf::Drawable
	{
	protected:
		sf::FloatRect AABB;
	public:
		bool onScreen = false;
		float screen_y;

		virtual void update(const sf::Time& time, const sf::Time& dt, Wind& wind);

		virtual void drawPre(sf::RenderTarget& target) const;

		virtual const sf::FloatRect& getAABB();
	};

}