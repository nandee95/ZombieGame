#include "../System/ResourceManager.hpp"


//Resource Manager

ResourceManager::~ResourceManager()
{
	for (auto& skeleton : spineSkeletons)
	{
		spSkeletonData_dispose(skeleton.second);
	}
	for (auto& atlas : spineAtlases)
	{
		spAtlas_dispose(atlas.second);
	}
}

const bool ResourceManager::isFileExists(std::string filename)
{
	std::ifstream fs(filename);
	return fs.good();
}

sf::Texture & ResourceManager::addTexture(std::string filename, int flags)
{
	if (this->existsTexture(filename)) return this->getTexture(filename);
	this->textures.insert(std::make_pair(filename, sf::Texture()));
	sf::Texture& texture = this->getTexture(filename);
	if (!isFileExists(filename)||!texture.loadFromFile(filename))
	{
		Error::Critical("Failed to load texture:\n" + filename);
	}
	if (flags & TextureFlags::TextureMipmap) texture.generateMipmap();
	if (flags & TextureFlags::TextureSmooth) texture.setSmooth(true);
	if (flags & TextureFlags::TextureRepeat) texture.setRepeated(true);
	return texture;
}

void ResourceManager::removeTexture(std::string filename)
{
	this->textures.erase(filename);
}

bool ResourceManager::existsTexture(std::string filename)
{
	return this->textures.count(filename) == 1;
}

sf::Texture& ResourceManager::getTexture(std::string filename)
{
	return this->textures.at(filename);
}

//Font

sf::Font & ResourceManager::addFont(std::string filename)
{
	if (this->existsFont(filename)) return this->getFont(filename);
	this->fonts.insert(std::make_pair(filename, sf::Font()));
	sf::Font& font = this->getFont(filename);
	if (!isFileExists(filename) || !font.loadFromFile(filename))
	{
		Error::Critical("Failed to load font:\n" + filename);
	}
	return font;
}

void ResourceManager::removeFont(std::string filename)
{
	this->fonts.erase(filename);
}

bool ResourceManager::existsFont(std::string filename)
{
	return this->fonts.count(filename) == 1;
}

sf::Font & ResourceManager::getFont(std::string filename)
{
	return this->fonts.at(filename);
}


//Music

sf::SoundBuffer & ResourceManager::addSound(std::string filename)
{
	if (this->existsSound(filename)) return this->getSound(filename);
	this->sounds.insert(std::make_pair(filename, sf::SoundBuffer()));
	sf::SoundBuffer& sound = this->getSound(filename);
	if (!isFileExists(filename) || !sound.loadFromFile(filename))
	{
		Error::Critical("Failed to load Sound:\n" + filename);
	}
	return sound;
}

void ResourceManager::removeSound(std::string filename)
{
	this->sounds.erase(filename);
}

bool ResourceManager::existsSound(std::string filename)
{
	return this->sounds.count(filename) == 1;
}

sf::SoundBuffer & ResourceManager::getSound(std::string filename)
{
	return this->sounds.at(filename);
}

//Spine - Atlas

spAtlas * ResourceManager::addSpineAtlas(std::string filename)
{
	if (existsSpineAtlas(filename)) return getSpineAtlas(filename);
	if (!isFileExists(filename))
	{
		Error::Critical("Failed to load Spine atlas:\n" + filename);
	}
	spAtlas* atlas = spAtlas_createFromFile(filename.c_str(), NULL);
	if (atlas == nullptr)
	{
		Error::Critical("Failed to load Spine atlas:\n" + filename);
	}
	this->spineAtlases.insert(std::make_pair(filename, atlas));
	return getSpineAtlas(filename);
}

bool ResourceManager::existsSpineAtlas(std::string filename)
{
	return spineAtlases.count(filename) == 1;
}

spAtlas * ResourceManager::getSpineAtlas(std::string filename)
{
	return spineAtlases.at(filename);
}

void ResourceManager::removeSpineAtlas(std::string filename)
{
	spAtlas_dispose(getSpineAtlas(filename));
	this->spineAtlases.erase(filename);
}

//Spine - Skeleton

spSkeletonData * ResourceManager::addSpineSkeleton(std::string filename, spAtlas * atlas, float scale)
{
	if (this->existsSpineSkeletion(filename)) return this->getSpineSkeletion(filename);

	if (!isFileExists(filename))
	{
		Error::Critical("Failed to load Spine skeleton:\n" + filename);
	}

	spSkeletonBinary* binary = spSkeletonBinary_create(atlas);
	binary->scale = scale;

	spSkeletonData* skeletonData = spSkeletonBinary_readSkeletonDataFile(binary, filename.c_str());
	if (!skeletonData)
	{
		Error::Critical("Failed to load Spine skeleton:\n" + filename);
	}

	spSkeletonBinary_dispose(binary);

	this->spineSkeletons.insert(std::make_pair(filename, skeletonData));
	return skeletonData;
}

bool ResourceManager::existsSpineSkeletion(std::string filename)
{
	return this->spineSkeletons.count(filename) == 1;
}

spSkeletonData * ResourceManager::getSpineSkeletion(std::string filename)
{
	return this->spineSkeletons.at(filename);
}

void ResourceManager::removeSpineSkeleton(std::string filename)
{
	this->spineSkeletons.erase(filename);
}
