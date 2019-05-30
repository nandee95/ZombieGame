#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../System/Error.hpp"
#include <unordered_map>
#include <memory>
#include <spine/Atlas.h>
#include <spine/SkeletonData.h>
#include <spine/SkeletonBinary.h>
#include <fstream>

class ResourceManager
{
private:
	//SFML
	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, sf::SoundBuffer> sounds;
	std::unordered_map<std::string, sf::Font> fonts;

	//Spine
	std::unordered_map<std::string, spAtlas*> spineAtlases;
	std::unordered_map<std::string, spSkeletonData*> spineSkeletons;

public:
	//Resource Manager
	~ResourceManager();

	//Statics

	static const bool isFileExists(std::string filename);

	//Texture
	enum TextureFlags : char
	{
		TextureSmooth=0x1,
		TextureMipmap=0x2,
		TextureRepeat=0x4
	};
	sf::Texture& addTexture(std::string filename, int flags = TextureFlags::TextureSmooth);
	void removeTexture(std::string filename);
	bool existsTexture(std::string filename);
	sf::Texture& getTexture(std::string filename);

	//Font
	sf::Font& addFont(std::string filename);
	void removeFont(std::string filename);
	bool existsFont(std::string filename);
	sf::Font& getFont(std::string filename);

	//Music
	sf::SoundBuffer& addSound(std::string filename);
	void removeSound(std::string filename);
	bool existsSound(std::string filename);
	sf::SoundBuffer& getSound(std::string filename);

	//Spine - Atlas
	spAtlas* addSpineAtlas(std::string filename);

	bool existsSpineAtlas(std::string filename);

	spAtlas* getSpineAtlas(std::string filename);

	void removeSpineAtlas(std::string filename);

	//Spine - Skeleton
	spSkeletonData* addSpineSkeleton(std::string filename, spAtlas* atlas, float scale = 1.0f);

	bool existsSpineSkeletion(std::string filename);

	spSkeletonData* getSpineSkeletion(std::string filename);

	void removeSpineSkeleton(std::string filename);
};
