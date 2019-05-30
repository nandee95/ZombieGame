#pragma once
#include <string>
#include <array>
#include <SFML/Graphics.hpp>
#include "../System/Math.hpp"
#include <iostream>
#include <memory>
#include "../System/ResourceManager.hpp"
#include <SFML/Network/Packet.hpp>
#include "../Entities/Entity.hpp"
#include "../Entities/Tree.hpp"
#include "../Entities/Grass.hpp"
#include "../Server/Protocol.hpp"

namespace client
{
	class Level;

	class Tile : public sf::Drawable
	{
	private:
	protected:
		friend Level;
		sf::VertexArray va;
		sf::Texture* texture;
		sf::Vector2f position;
	public:
		Protocol::TileTexture type;
		Tile();

		void setPosition(sf::Vector2f& position);

		void setTexture(sf::Texture& texture);

		virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const;
	};

	class ChunkResources
	{
	private:
		const std::array<std::string, 2U> tileTextures = {
			"sprites/environment/t_grass.png","sprites/environment/t_water.png"
		};
	public:
		ResourceManager& resMg;
		ChunkResources(ResourceManager& resMg);

		sf::Texture& get(unsigned char id);
	};
	class Chunk
	{
	protected:
		std::array<std::array<Tile,10U>,10U> tiles;


		sf::Vector2i id;

		Physics& physics;
		ChunkResources& chunkMg;
		ResourceManager& resMg;
	public:
		std::vector<std::shared_ptr<client::Entity>> entities;
		const float tileWidth = 128.0f;
		const float tileHeight = 64.0f;
		Chunk(ChunkResources& chunkMg, sf::Vector2i id, Physics& physics, ResourceManager& resMg);

		friend sf::Packet& operator >> (sf::Packet& packet, client::Chunk& chunk);

		void drawTile(sf::RenderTarget& target, sf::Vector2i tile);

		void drawEntities(sf::RenderTarget& target);
	};
}