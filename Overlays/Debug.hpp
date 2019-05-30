#pragma once
#include <SFML\Graphics.hpp>
#include <unordered_set>
#include <SFML/OpenGL.hpp>
#include "../System/ResourceManager.hpp"
#include "../Client/Wind.hpp"

class DebugOverlay : public sf::Drawable
{
private:
	sf::Text info;
	//Wind
	sf::Vector2f center;
	sf::View& view;

	std::string vendor;
	std::string renderer;
public:
	struct Vec2iHash
	{
		size_t operator () (const sf::Vector2i& vec) const
		{
			return vec.x * 10000 + vec.y;
		}
	};

	DebugOverlay(ResourceManager& resMg, sf::View& view);


	bool chunkIntersectsScreen(sf::Vector2i chunk, sf::FloatRect screen);

	void update(sf::Time dt, sf::Vector2f playerPosition, Wind& wind, int entityCount, int totalEntities);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};