#include "../Server/Chunk.hpp"
#include <iostream>

server::Chunk::Chunk(sf::Vector2i chunk) :id(chunk)
{
	if (exists(chunk)) this->load();
	else this->create();
}

server::Chunk::~Chunk()
{
	this->save();
}

void server::Chunk::update()
{

}

void server::Chunk::create()
{
	//Generate random seed
	std::uniform_int_distribution<int> seedDist(INT_MIN, INT_MAX);
	int seed = seedDist(std::default_random_engine());


	float biomeScale = 1.0f;
	//Handle map definition file
	{
		CfgFile cfg("Save/worlddef.cfg");
		cfg.validate("World", "Seed", std::to_string(seed), Validate::Int);
		cfg.validate("World", "Name", "New World", Validate::Any);
		cfg.validate("Settings", "BiomeScale", "100", Validate::Int);

		cfg.cleanUp();
		cfg.update();

		//Storing settings
		seed = cfg.getValue("World", "Seed").asInt();
		biomeScale = static_cast<float>(cfg.getValue("Settings", "BiomeScale").asInt()) / 100.0f;
	}

	//Generate map
	noise::module::Voronoi biomeGenerator;

	biomeGenerator.SetSeed(seed);
	biomeGenerator.SetFrequency(0.1*biomeScale);

	std::default_random_engine biomeEngine;
	biomeEngine.seed(seed + id.x * 1 << 16 + id.y);

	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			const sf::Vector2i position = id * 10 + sf::Vector2i(x, y);
			const sf::Vector2f positionalSeed(sf::Vector2f(position)*0.942672f); //libnoise works better with not round values

			double biomeRaw = biomeGenerator.GetValue(positionalSeed.x, positionalSeed.y, 521.2512510f);

			int biome = static_cast<int>(std::floor((biomeRaw + 1.0) / 2.0*100.0));

			if (biome < 40)
			{
				/*
				40% Forest Biome:

				1. Tree
				1:3 chance that a forest Tile contains tree
				CONDITION: Random positions, other tree no closer than 1 blocks

				2. Grass:
				by perlin noise, treshold of 0.35, 1:5 probability
				CONDITIONS: Tree no closer than 0.25 blocks, Grass no closer than 0.05 blocks
				*/
				tiles[x][y].type = Protocol::TileTexture::TextureGrass;
			}
			else
			{
				tiles[x][y].type = Protocol::TileTexture::TextureForestGrass;
			}
		}
	}


	std::uniform_int_distribution<int> treeDist(0, 2);
	std::uniform_int_distribution<int> grassDist(2, 5);
	std::uniform_int_distribution<int> grassDist2(10, 15);
	std::uniform_real_distribution<float> precisionDist(0.0f, 1.0f);

	//First pass

	for (char x = 0; x < 10; x++)
	{
		for (char y = 0; y < 10; y++)
		{
			switch (tiles[x][y].type)
			{
			case Protocol::TileTexture::TextureGrass:
			{
				if (treeDist(biomeEngine) == 0)
				{
					const sf::Vector2f position = sf::Vector2f(x, y) + sf::Vector2f(precisionDist(biomeEngine), precisionDist(biomeEngine));

					for (auto& e : entities)
					{
						if (e->getId() != Protocol::Entity::Tree) continue;
						if (Math::distance(position, e->getPosition()) < 1.0f) goto skip;
					}
					this->entities.push_back(std::make_shared<server::Tree>(server::Tree::Type1, position));
				}

			skip: {}
			} break;
			case Protocol::TileTexture::TextureForestGrass:
			{
				const int grassCount = grassDist2(biomeEngine);
				for (int i = 0; i < grassCount; i++)
				{
					const sf::Vector2f position = sf::Vector2f(x, y) + sf::Vector2f(precisionDist(biomeEngine), precisionDist(biomeEngine));
					this->entities.push_back(std::make_shared<server::Grass>(server::Grass::Type1, position));
				}

			} break;
			}
		}
	}

	//Second pass

	for (char x = 0; x < 10; x++)
	{
		for (char y = 0; y < 10; y++)
		{
			switch (tiles[x][y].type)
			{
			case Protocol::TileTexture::TextureGrass:
			{
				const int grassCount = grassDist(biomeEngine);
				for (int i = 0; i < grassCount; i++)
				{
					const sf::Vector2f position = sf::Vector2f(x, y) + sf::Vector2f(precisionDist(biomeEngine), precisionDist(biomeEngine));
					this->entities.push_back(std::make_shared<server::Grass>(server::Grass::Type1, position));
				}


			} break;
			case Protocol::TileTexture::TextureForestGrass:
			{

			} break;
			}
		}
	}




	save();
}

void server::Chunk::load()
{
	std::ifstream fs(fileName(id));
	for (auto& tt : tiles)
	{
		for (auto& t : tt)
		{
			fs >> t;
		}
	}

}

void server::Chunk::save()
{
	std::ofstream fs(fileName(id), std::ifstream::trunc);
	for (auto& tt : tiles)
	{
		for (auto& t : tt)
		{
			fs << t;
		}
	}

}

bool server::Chunk::exists(sf::Vector2i & chunk)
{
	std::fstream fs(fileName(chunk));
	return fs.good();
}

std::string server::Chunk::fileName(sf::Vector2i chunk)
{
	return "Save/Chunks/" + std::to_string(chunk.x) + ";" + std::to_string(chunk.y) + ".chunk";
}
