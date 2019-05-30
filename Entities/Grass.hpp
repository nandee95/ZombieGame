#pragma once

#include "../Entities/Entity.hpp"
#include "../System/Math.hpp"
#include "../System/ResourceManager.hpp"
#include <iostream>
#include <array>

namespace client
{
	class Grass : public Entity
	{
	private:
		const float scale = 2.0f;
		//Shader
		static sf::Shader* shader;
		static int shaderUsers;

		//Etc
		sf::VertexArray va;
		sf::Texture& texture;
		sf::Vector2f size;

		sf::Vector2f strength;

	public:
		Grass(std::vector<sf::Vector2f> positions, sf::Vector2f resolution, ResourceManager& resMg);

		~Grass();

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

		virtual void update(const sf::Time& time, const sf::Time& dt, Wind& wind);
	};

}
