#include "../Client/Client.hpp"
#include "../System/Error.hpp"


Client::Client(Window& window, CfgFile& cfg) : window(window) ,
	reporterThread(&Client::Reporter, this),
	listenerThread(&Client::Listener, this),
	chunkMg(resMg),
	wind(resMg)
{
	inventories.push_back(std::make_shared<DefaultInventory>(resMg,window.getSize()));

	Input playerInput;
	playerInput.set(PlayerAction::MoveLeft, cfg.getValue("Controls", "MoveLeft").string);
	playerInput.set(PlayerAction::MoveRight, cfg.getValue("Controls", "MoveRight").string);
	playerInput.set(PlayerAction::MoveUp, cfg.getValue("Controls", "MoveUp").string);
	playerInput.set(PlayerAction::MoveDown, cfg.getValue("Controls", "MoveDown").string);
	playerInput.set(PlayerAction::Run, cfg.getValue("Controls", "Run").string);

	this->view = window.getDefaultView();

	entities.insert(std::make_pair(0.0f, std::make_shared<SmokeParticleEmitter>(sf::Vector2f(1,1 ), resMg)));

	this->player = std::make_shared<LocalPlayer>(sf::Vector2f(),physics,playerInput,this->view,resMg);
	entities.insert(std::make_pair(10.0f, player));

	if (server.connect(cfg.getValue("Client", "Ip").string, cfg.getValue("Client", "Port").asInt()) != sf::Socket::Done)
	{
		//Error::Critical("Failed to connect to the remote server!");
	}

	sf::Packet details;
	details << (short)Protocol::ConnectDetails;
	std::string name = cfg.getValue("Gameplay", "Name").string;
	details << std::wstring(name.begin(),name.end());
	this->sendPacket(details);
	
	this->chat = std::make_unique<Chat>(resMg.addFont("fonts/Arial.ttf"), this->server);

	const sf::Vector2u& size = window.getSize();
	target.create(static_cast<float>(size.x), static_cast<float>(size.y));
	reporterThread.detach();
	listenerThread.detach();
}

Client::~Client()
{
	this->exitFlag = true;
	this->listenerThread.join();
	this->reporterThread.join();
}

void Client::Listener()
{
	while (!this->exitFlag)
	{
		sf::Packet packet;
		this->server.receive(packet);
		short protocol;
		if (packet >> protocol)
		{
			std::lock_guard<std::mutex> guard(mu);
			if(protocol!=Protocol::PlayerUpdate)
			std::cout << "Client packet received: " << protocol << std::endl;
			switch (protocol)
			{
			case Protocol::Message:
			{
				std::wstring message;
				if (packet >> message)
				{
					this->chat->printMessage(message);
				}
			}break;
			case Protocol::PlayerConnect:
			{
				int id;
				std::wstring name;
				if (packet >> id >> name)
				{
					remotePlayers.insert(std::make_pair(id,
						std::make_shared<RemotePlayer>(sf::Vector2f(), physics, resMg)
						));
					remotePlayers.at(id)->setName(name);
					entities.insert(std::make_pair(0.0f,remotePlayers.at(id)));
				}
			} break;
			case Protocol::PlayerUpdate:
			{
				int id;
				float posx, posy;
				std::string anim;
				bool mirrored;
				if (packet >> id >> posx >> posy >> anim >> mirrored)
				{
					if (remotePlayers.count(id) != 0)
					{
						remotePlayers.at(id)->setPositionInterpolation(timer.getElapsedTime(),sf::Vector2f(posx, posy));
						remotePlayers.at(id)->setAnimation(anim);
						remotePlayers.at(id)->setMirrored(mirrored);
					}
				}
			}break;
			case Protocol::PlayerDisconnect:
			{
				int id;
				if (packet >> id)
				{
					for (auto it = entities.begin() ; it!=entities.end();it++)
					{
						if (it->second == remotePlayers.at(id))
						{
							entities.erase(it);
							break;
						}
					}
					remotePlayers.erase(id);
				}
			} break;
			case Protocol::ChunkResponse:
			{
				int posx, posy;
				if (packet >> posx >> posy)
				{
					Chunk chunk(chunkMg, sf::Vector2i(posx, posy),physics,resMg);
					if (packet >> chunk)
					{
						chunks.insert(std::make_pair(sf::Vector2i(posx, posy), chunk));
						int count = 0;
						for (auto& e : chunks.at(sf::Vector2i(posx, posy)).entities)
						{
							registerEntity(e);
							count++;
						}
					}
				}
			} break;
			}
		}
	}
}

void Client::update()
{
	sf::Time time = timer.getElapsedTime();
	sf::Time dt = time - lastFrame;
	lastFrame = time;

	this->wind.update(time);
	this->physics.update(dt);

	sf::FloatRect screenAABB(view.getCenter()-view.getSize()/2.0f, view.getSize());
	int count = 0;
	for (auto it = entities.begin();it!=entities.end();)
	{
		it->second->onScreen = AABBintersectionOptimized(screenAABB,it->second->getAABB());


		if (it->second->onScreen)
			count++;
		if (it->first != it->second->screen_y)
		{
			auto ptr = it->second;
			it = entities.erase(it);
			entities.insert(std::make_pair(ptr->screen_y,ptr));
		}
		else it++;
	}


	if (showDebug)
	{
		this->debugScreen->update(dt,this->player->getPosition(), wind,count,entities.size());
	}

	if (!player->onScreen)
	{
		player->update(time, dt, wind);
	}

	for (auto& e : entities)
	{
		//if(e.second->onScreen)
		e.second->update(time,dt,wind);
	}

	if (player)
	{
		const sf::Vector2i chunk = Math::worldToChunk(player->getPosition());
		if (chunk != lastChunk)
		{
			std::cout << "chunkchange " << std::endl;
			for (int x = -1; x <= 1; x++)
			{
				for (int y = -1; y <= 1; y++)
				{
					if (chunks.find(chunk+sf::Vector2i(x,y))==chunks.end())
					{
						sf::Packet request;
						request << (short)Protocol::RequestChunk;
						request << (chunk.x + x) << (chunk.y + y);
						server.send(request);
						
					}
				}
			}

			for (auto it = chunks.begin(); it != chunks.end(); )
			{
				if (std::abs(it->first.x - chunk.x) > 1 ||
					std::abs(it->first.y - chunk.y) > 1)
				{
					for (auto it2 = this->entities.begin(); it2 != this->entities.end(); )
					{
						bool found = false;
						for (auto& e : it->second.entities)
						{
							if (e == it2->second)
							{
								found = true;
								break;
							}
						}
						if (found)
						{
							it2 = this->entities.erase(it2);
						}
						else it2++;
					}
					it = chunks.erase(it);
				}
				else it++;
			}
			lastChunk = chunk;
		}
	}
}

void Client::draw(sf::RenderTarget& _target)
{
	std::lock_guard<std::mutex> guard(mu);
	target.clear(sf::Color::White);
	target.setView(view);

	sf::Vector2f min = view.getCenter() - (view.getSize() / 2.0f);
	sf::Vector2f max = view.getCenter() + (view.getSize() / 2.0f);
	for (int a = static_cast<int>(std::floor(min.y / 64.0f)) - 2; a < static_cast<int>(std::ceil(max.y / 64.0f))+2; a++) //Start: top
	{
		for (int b = static_cast<int>(std::floor(min.x / 128.0f)) - 2; b < static_cast<int>(std::ceil(max.x / 128.0f))+2; b++) //Start: right
		{
			if ((b & 1) != (a & 1)) continue;
			const sf::Vector2i position((a - b) / 2, (a + b) / 2);
			const sf::Vector2i chunk= Math::worldToChunk(sf::Vector2f(position));

			if (!isChunkLoaded(chunk)) continue;

			if (chunks.find(chunk) != chunks.end())
			{
				chunks.at(chunk).drawTile(target, position - chunk*10);
			}
		}
	}
	
	if (showDebug)
	{
		sf::VertexArray va(sf::Lines);
		for (int x = -50; x < 50; x += 10)
		{
			for (int y = -50; y < 50; y += 10)
			{
				va.append(sf::Vertex(Math::worldToScreen(sf::Vector2f(x, y)), sf::Color::Yellow));
				va.append(sf::Vertex(Math::worldToScreen(sf::Vector2f(x + 10, y)), sf::Color::Yellow));
				va.append(sf::Vertex(Math::worldToScreen(sf::Vector2f(x + 10, y)), sf::Color::Yellow));
				va.append(sf::Vertex(Math::worldToScreen(sf::Vector2f(x + 10, y + 10)), sf::Color::Yellow));
				va.append(sf::Vertex(Math::worldToScreen(sf::Vector2f(x + 10, y + 10)), sf::Color::Yellow));
				va.append(sf::Vertex(Math::worldToScreen(sf::Vector2f(x, y + 10)), sf::Color::Yellow));
				va.append(sf::Vertex(Math::worldToScreen(sf::Vector2f(x, y + 10)), sf::Color::Yellow));
				va.append(sf::Vertex(Math::worldToScreen(sf::Vector2f(x, y)), sf::Color::Yellow));
			}
		}

		target.draw(va);
	}
	
	/*for (auto& e : this->entities)
	{
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(e.second->getAABB().width, e.second->getAABB().height));
		rect.setPosition(sf::Vector2f(e.second->getAABB().left, e.second->getAABB().top));
		rect.setOutlineThickness(1);
		rect.setOutlineColor(sf::Color::Red);
		rect.setFillColor(sf::Color::Transparent);
		target.draw(rect);
	}*/

	for (auto& e : this->entities)
	{
		//if(e.second->onScreen)
		e.second->drawPre(target);
	}
	for (auto& e : this->entities)
	{
		//if (e.second->onScreen)
		target.draw(*e.second);
	}
	//UI
	target.setView(target.getDefaultView());
	target.draw(*this->chat);
	
	if (showDebug)
	{
		target.draw(*debugScreen);
	}

	if (inventory)
	{
		for (auto& i : inventories)
		{
			target.draw(*i);
		}
	}

	target.display();
	_target.draw(sf::Sprite(target.getTexture()));
}

void Client::Reporter()
{
	sf::sleep(sf::milliseconds(500));
	while (!this->exitFlag)
	{
		sf::sleep(sf::milliseconds(50));
		std::lock_guard<std::mutex> guard(mu);
		sf::Packet packet;
		packet << (short)Protocol::PlayerUpdate;
		const sf::Vector2f pos = player->getPosition();
		bool mirrored = player->isMirrored();
		packet << pos.x << pos.y << player->getAnimation() << mirrored;
		sendPacket(packet);
	}
}

bool Client::isChunkLoaded(sf::Vector2i chunk)
{
	return this->chunks.count(chunk) == 1;
}

void Client::sendPacket(sf::Packet & packet)
{
	this->server.send(packet);
}

void Client::registerEntity(std::shared_ptr<client::Entity> entity)
{
	this->entities.insert(std::make_pair(entity->screen_y, entity));
}

void Client::destroyEntity(std::shared_ptr<client::Entity> entity)
{
	for (auto it = entities.begin(); it != entities.end(); it++)
	{
		if (it->second == entity)
		{
			this->entities.erase(it);
			break;
		}
	}
}

const bool Client::AABBintersectionOptimized(const sf::FloatRect & rect1, const sf::FloatRect & rect2)
{
	return !(
		rect1.left + rect1.width < rect2.left || rect2.left + rect2.width < rect1.left ||
		rect1.top + rect1.height < rect2.top || rect2.top + rect2.height < rect1.top
		);
}



void Client::event(sf::Event& e)
{
	if (this->chat->event(e)) return;
	this->player->event(e);
	for (auto& i : inventories)
	{
		i->event(e);
	}
	if (e.type == sf::Event::KeyPressed)
	{
		switch(e.key.code)
		{
		case sf::Keyboard::Escape:
		{
			exit(0);
		} break;
		case sf::Keyboard::F1:
		{
			if (this->debugScreen == nullptr)
			{
				this->debugScreen = std::make_unique<DebugOverlay>(resMg,this->view);
			}
			showDebug = !showDebug;
		} break;
		case sf::Keyboard::I:
		{
			inventory = !inventory;
		} break;
		case sf::Keyboard::Space:
		{
			if (!inventories.at(0)->pushItem(std::make_shared<EnergyDrink>(resMg, 0.5f))) std::cout << "Falied to insert item";
		} break;
		}
	}
}