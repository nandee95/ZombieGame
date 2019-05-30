#pragma once

#include "Protocol.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Network/Packet.hpp>
#include <fstream>

namespace server
{
	class Entity
	{
	protected:
		short id = -1;
		sf::Vector2f position;
	public:

		const short& getId() const
		{
			return this->id;
		}

		const sf::Vector2f& getPosition()
		{
			return this->position;
		}

		void setPosition(sf::Vector2f position)
		{
			this->position = position;
		}

		virtual void streamInto(sf::Packet& packet) = 0;
		virtual void streamToFile(std::ofstream& file) = 0;
		virtual void streamFromFile(std::ifstream& file) = 0;
		//virtual friend std::ofstream& operator <<(std::ofstream& fs, const Entity& t) = 0;

	};

	class Tree : public Entity
	{
	public:
		enum TreeType
		{
			Type1,
			Type2,
			Type3
		};
	private:
		TreeType type;
	public:
		Tree(TreeType type, sf::Vector2f position) : type(type)
		{
			id = Protocol::Entity::Tree;
			this->position = position;
		}

		virtual void streamInto(sf::Packet& packet)
		{
			packet << this->id << this->position.x << this->position.y << (short)this->type;
		}
		virtual void streamToFile(std::ofstream& file)
		{
			file << this->id << this->position.x << this->position.y << (short)this->type;
		}

		virtual void streamFromFile(std::ifstream& file)
		{
			short t = 0;
			file >> this->id >> this->position.x >> this->position.y >> t;
			this->type = static_cast<TreeType>(t);
		}

	};

	class Grass : public Entity
	{
	public:
		enum GrassType
		{
			Type1,
			Type2,
			Type3
		};
	private:
		GrassType type;
	public:
		Grass(GrassType type, sf::Vector2f position) : type(type)
		{
			id = Protocol::Entity::Grass;
			this->position = position;
		}

		virtual void streamInto(sf::Packet& packet)
		{
			packet << this->id << this->position.x << this->position.y << (short)this->type;
		}

		virtual void streamToFile(std::ofstream& file)
		{
			file << this->id << this->position.x << this->position.y << (short)this->type;
		}

		virtual void streamFromFile(std::ifstream& file)
		{
			short t=0;
			file >> this->id >> this->position.x >> this->position.y >> t;
			this->type = static_cast<GrassType>(t);
		}

	};
}