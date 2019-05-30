#pragma once

#include "../System/State.hpp"
#include "../System/CfgFile.hpp"
#include "../Client/Chat.hpp"
#include "../Client/Wind.hpp"
#include "../Server/Protocol.hpp"
#include "../System/ResourceManager.hpp"
#include "../Client/Level.hpp"
#include "../System/Window.hpp"
#include "../Physics/Physics.hpp"
#include "../Overlays/Debug.hpp"
#include "../Entities/Tree.hpp"
#include "../Entities/ParticleEmitter.hpp"
#include "../Entities/Grass.hpp"
#include "../Inventory/Inventory.hpp"
#include "../Client/Player.hpp"

#include <unordered_map>
#include <thread>
#include <mutex>

using namespace client;

struct Vec2iHash
{
	size_t operator () (const sf::Vector2i& vec) const
	{
		return vec.x * 10000 + vec.y;
	}
};

class Client : public State
{
protected:
	ResourceManager resMg;
	sf::TcpSocket server;
	std::unique_ptr<Chat> chat;
	bool exitFlag = false;
	std::thread physicsThread;
	std::shared_ptr<client::LocalPlayer> player;
	sf::Clock timer;
	sf::Time lastFrame;
	Physics physics;

	sf::View view;
	bool showDebug = false;
	std::unique_ptr<DebugOverlay> debugScreen;

	sf::RenderTexture target;

	std::multimap<float, std::shared_ptr<client::Entity>> entities;

	Wind wind;

	std::thread reporterThread,listenerThread;

	std::map<int, std::shared_ptr<client::RemotePlayer>> remotePlayers;

	std::vector<std::shared_ptr<Inventory>> inventories;
	bool inventory = false;

	ChunkResources chunkMg;
	std::unordered_map<const sf::Vector2i, client::Chunk,Vec2iHash> chunks;
	sf::Vector2i lastChunk=sf::Vector2i(-5125,-15672);

	std::mutex mu;
public:
	Window& window;
	Client(Window& window,CfgFile& cfg);
	~Client();
	void Listener();

	virtual void event(sf::Event& e);
	virtual void update();
	virtual void draw(sf::RenderTarget& _target);

	void Reporter();

	bool isChunkLoaded(sf::Vector2i chunk);

	void sendPacket(sf::Packet& packet);

	void registerEntity(std::shared_ptr<client::Entity> entity);

	void destroyEntity(std::shared_ptr<client::Entity> entity);

	const bool AABBintersectionOptimized(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
};