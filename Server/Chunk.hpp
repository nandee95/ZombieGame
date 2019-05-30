#pragma once

#include "../System/Math.hpp"
#include "../Server/Entity.hpp"
#include "../System/CfgFile.hpp"
#include "../Server/Protocol.hpp"

#include <SFML/System/Vector2.hpp>

#include <noise\noise.h>

#include <random>
#include <memory>
#include <array>
#include <fstream>
#include <unordered_map>

namespace server
{
	struct Tile
	{
		Protocol::TileTexture type;
		Protocol::Biome biome;

		friend sf::Packet& operator << (sf::Packet& packet, Tile& t)
		{
			packet << (unsigned char)t.type;
			return packet;
		}

		friend std::ofstream& operator <<(std::ofstream& fs, const Tile& t)
		{
			fs << (unsigned char)t.type;
			return fs;
		}
		
		friend std::ifstream& operator >>(std::ifstream& fs, Tile& t)
		{
			unsigned char temp;
			fs >> temp;
			t.type = static_cast<Protocol::TileTexture>(temp);
			return fs;
		}
	};

	class Chunk
	{
	private:
		enum Texture
		{
			Grass,Water,Mountain
		};
	private:
		const sf::Vector2i id;
		std::array<std::array<Tile, 10U>, 10U> tiles;
		std::vector<std::shared_ptr<server::Entity>> entities;
	public:
		Chunk(sf::Vector2i chunk);

		~Chunk();

		void update();

		friend sf::Packet& operator << (sf::Packet& packet, server::Chunk& chunk)
		{
			packet << chunk.id.x << chunk.id.y;
			for (auto& tt : chunk.tiles)
			{
				for (auto& t : tt)
				{
					packet << t;
				}
			}

			for (auto& ent : chunk.entities)
			{
				ent->streamInto(packet);
			}
			packet << (short)-1;
			return packet;
		}

		void create();
		void load();
		void save();

		static bool exists(sf::Vector2i& chunk);
		static std::string fileName(sf::Vector2i chunk);
	};

};