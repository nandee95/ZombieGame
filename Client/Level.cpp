#include "../Client/Level.hpp"

client::Tile::Tile()
{
	this->va.setPrimitiveType(sf::Quads);

	this->va.append(sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 0)));
	this->va.append(sf::Vertex(sf::Vector2f(128, 64), sf::Vector2f(256, 0)));
	this->va.append(sf::Vertex(sf::Vector2f(0, 128), sf::Vector2f(256, 256)));
	this->va.append(sf::Vertex(sf::Vector2f(-128, 64), sf::Vector2f(0, 256)));
}

void client::Tile::setPosition(sf::Vector2f & position)
{
	this->position = position;
}

void client::Tile::setTexture(sf::Texture & texture)
{
	this->texture = &texture;
}

void client::Tile::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.texture = texture;
	states.transform.translate(position);
	target.draw(va, states);
}

client::ChunkResources::ChunkResources(ResourceManager & resMg) : resMg(resMg)
{
	for (auto& t : tileTextures)
	{
		sf::Texture& tex = resMg.addTexture(t);
		if (tex.getSize() != sf::Vector2u(256U, 256U))
		{
			std::cout << "invalid tile size" << std::endl;
		}
	}
}

sf::Texture & client::ChunkResources::get(unsigned char id)
{
	return resMg.addTexture(tileTextures.at(id));
}

client::Chunk::Chunk(ChunkResources & chunkMg, sf::Vector2i id, Physics & physics, ResourceManager & resMg) : chunkMg(chunkMg), id(id), physics(physics), resMg(resMg)
{
}

void client::Chunk::drawTile(sf::RenderTarget & target, sf::Vector2i tile)
{
	target.draw(tiles.at(tile.x).at(tile.y));
}

void client::Chunk::drawEntities(sf::RenderTarget & target)
{
	for (auto& e : entities)
	{
		target.draw(*e);
	}

}

sf::Packet & client::operator>>(sf::Packet & packet, client::Chunk & chunk)
{
	char x = 0;
	for (auto& tt : chunk.tiles)
	{
		char y = 0;
		for (auto& t : tt)
		{
			unsigned char type;
			if (packet >> type)
			{
				t.setPosition(Math::worldToScreen(sf::Vector2f(chunk.id)*10.0f + sf::Vector2f(x, y)));
				t.type = static_cast<Protocol::TileTexture>(type);
				t.setTexture(chunk.chunkMg.get(type));
			}
			y++;
		}
		x++;
	}

	short entid;
	int count = 0;
	std::vector<sf::Vector2f> grassPoses;
	while (packet >> entid && entid != -1)
	{
		switch (entid)
		{
		case Protocol::Entity::Tree:
		{
			float x, y;
			short type;
			if (packet >> x >> y >> type)
			{
				chunk.entities.push_back(std::make_shared<client::Tree>(sf::Vector2f(chunk.id)*10.0f + sf::Vector2f(x, y), chunk.resMg, chunk.physics, sf::Vector2f(1280, 720)));
			}
		} break;
		case Protocol::Entity::Grass:
		{
			float x, y;
			short type;
			if (packet >> x >> y >> type)
			{
				grassPoses.push_back(sf::Vector2f(sf::Vector2f(chunk.id)*10.0f + sf::Vector2f(x, y)));
			}
		} break;
		}
		count++;
	}
	chunk.entities.push_back(std::make_shared<Grass>(grassPoses, sf::Vector2f(1280, 720), chunk.resMg));//TODO

	std::cout << count << "entity received" << std::endl;
	return packet;
}
