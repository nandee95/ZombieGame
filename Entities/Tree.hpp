#pragma once
#include "../System/ResourceManager.hpp"
#include "../Entities/Entity.hpp"
#include "../Physics/Physics.hpp"

namespace client
{
	class Tree : public client::Entity
	{
	private:
		const float bodyRadius = 12.0f;
		std::shared_ptr<CircleBody> body;
		sf::Sprite shadow;

		//Shader
		static sf::Shader* shader;
		static int shaderUsers;


		sf::VertexArray va;
		sf::Texture& texture;

		sf::Vector2f size;
		sf::Vector2f strength;
	public:
		Tree(sf::Vector2f position, ResourceManager& resMg, Physics& phy, sf::Vector2f resolution);

		virtual void drawPre(sf::RenderTarget& target) const;
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		virtual void update(const sf::Time& time, const sf::Time& dt, Wind& wind);
	};
}