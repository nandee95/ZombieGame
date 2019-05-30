#pragma once

#include <SFML/Network.hpp>
#include "../System/CfgFile.hpp"
#include "../Server/Protocol.hpp"
#include "../Server/Chunk.hpp"
#include <thread>
#include <memory>
#include <iostream>
#include <regex>
#include <mutex>

using namespace server;

struct RemoteClient
{
	//Server
	sf::TcpSocket socket;
	std::unique_ptr<std::thread> thread;
	//Data
	std::wstring name = L"unknown";
	bool connected = false;
	bool needsUpdate = true;
	int ping = 0;
	sf::Time lastPacket;

	//Model
	sf::Vector2f position;
	std::string animation;
	bool mirrored;

	void setAnimation(std::string animation)
	{
		this->animation = animation;
		needsUpdate = true;
	}

	void setPosition(sf::Vector2f position)
	{
		this->position = position;
		needsUpdate = true;
	}

	void setMirrored(bool mirrored)
	{
		this->mirrored = mirrored;
		needsUpdate = true;
	}

	void sendPacket(sf::Packet& packet)
	{
		this->socket.send(packet);
	}
};

class Server
{
private:
	struct Vec2iHash
	{
		size_t operator () (const sf::Vector2i& vec) const
		{
			return vec.x * 10000 + vec.y;
		}
	};
	bool ready = false;
	bool running = true;
	sf::TcpListener server;
	std::map<int, RemoteClient> clients;
	int clientsNumber = 0;
	//Thread that waits for new connections
	std::unique_ptr<std::thread> listenerThread;
	std::unique_ptr<std::thread> reporterThread;
	std::unordered_map<const sf::Vector2i, server::Chunk, Vec2iHash> chunks;
	sf::Time lastTick;
public:
	Server(CfgFile& cfg)
	{
#ifndef SERVER
		const int port = cfg.getValue("InternalServer", "Port").asInt();
#else
		const int port = cfg.getValue("Server", "Port").asInt();
#endif
		;
		const sf::Socket::Status status = server.listen(port);
		switch (status)
		{
		case sf::Socket::Status::Done:
		{
			std::cout << "Server starting at port: " << server.getLocalPort() << std::endl;
			ready = true;
		} break;
		default:
		{
			std::cout << "Failed to start server... (#"<< status <<")" << std::endl;
			running = false;
			return;
		} break;
		}

		listenerThread = std::make_unique<std::thread>(&Server::Listener, this);
		reporterThread = std::make_unique<std::thread>(&Server::Reporter, this);
	}

	~Server()
	{
		std::cout << "Server is shutting down..." << std::endl;
		sendMessageAll(L"Server is shutting down...");
		server.close();
		running = false;
		if(reporterThread && reporterThread->joinable()) reporterThread->join();
		if (listenerThread && listenerThread->joinable()) listenerThread->join();
		for (auto& c : clients)
		{
			if(c.second.thread && c.second.thread->joinable()) c.second.thread->join();
		}
	}

	const bool isReady() const
	{
		return this->ready;
	}

	const bool isRunning() const
	{
		return this->running;
	}

	void sendMessageAll(std::wstring message)
	{
		sf::Packet msgPacket;
		msgPacket << static_cast<short>(Protocol::Message) << message;
		for (auto& c : clients)
		{
			c.second.sendPacket(msgPacket);
		}
	}

	void Reporter()
	{
		while (running)
		{
			for (auto& c : clients)
			{
				if (!c.second.connected) continue;
				for (auto& c2 : clients)
				{
					if (c.first == c2.first || !c2.second.connected) continue; //Dont need info about self

					sf::Packet packet;
					packet << (short)Protocol::PlayerUpdate;
					packet << c2.first << c2.second.position.x << c2.second.position.y << c2.second.animation << (bool)c2.second.mirrored;
					c.second.socket.send(packet);
				}
			}
			sf::sleep(sf::milliseconds(50));
		}
	}

	void Listener() //New connections
	{
		while (running)
		{
			if (server.accept(clients[clientsNumber].socket) != sf::Socket::Done)
			{
				std::cout << "Connection failed from: " << clients[clientsNumber].socket.getRemoteAddress().toString() << std::endl;
				continue;
			}

			clients[clientsNumber].thread = std::make_unique<std::thread>(&Server::ClientListener, this, clientsNumber);

			std::cout << "Client #" << clientsNumber << " connected successfully! Waiting for login info..." << std::endl;

			for (auto& c : clients)
			{
				if (!c.second.connected || clientsNumber == c.first) continue;
				sf::Packet playerPacket;
				playerPacket << static_cast<short>(Protocol::PlayerConnect) << c.first << c.second.name;
				clients[clientsNumber].socket.send(playerPacket);
			}

			clientsNumber++;
		}
	}

	void ClientListener(int id) //One specific connection
	{
		RemoteClient& client = this->clients[id];
		while (true)
		{
			sf::Packet inPacket;
			sf::Socket::Status status = client.socket.receive(inPacket);
			if (status == sf::Socket::Done)
			{
				short protocol;
				if (inPacket >> protocol)
				{
					if (protocol != Protocol::PlayerUpdate)
						std::cout << "Packet received: " << protocol << std::endl;
					switch (protocol)
					{
					case Protocol::Message:
					{
						std::wstring message;
						if (inPacket >> message && client.connected)
						{
							sf::Packet outPacket;
							std::size_t found = message.find_first_of('\\');
							while (found != std::wstring::npos)
							{
								message.insert(found, 1, '\\');
								found = message.find_first_of('\\', found + 2);
							}
							std::cout << "Server, message received" << std::endl;
							message = L"\\A3FF7F" + client.name + L"\\CCFFCC : " + message;
							outPacket << static_cast<short>(Protocol::Message) << message;
							for (auto& c : this->clients)
							{
								c.second.socket.send(outPacket);
							}
						}
					} break;
					case Protocol::PlayerUpdate:
					{
						float posx, posy;
						std::string anim;
						bool mirrored;
						if (inPacket >> posx >> posy >> anim >> mirrored && client.connected)
						{
							client.position.x = posx;
							client.position.y = posy;
							client.animation = anim;
							client.mirrored = mirrored;
						}
					} break;
					case Protocol::ConnectDetails:
					{
						std::wstring name;
						if (inPacket >> name)
						{
							std::wstring fName = name;
							int count = 2;
							bool found = false;
							do
							{
								found = false;
								for (auto& c : clients)
								{
									if (c.second.name == fName)
									{
										found = true;
										break;
									}
								}

								if (found)
								{
									fName = name + L" (" + std::to_wstring(count) + L")";
								}
							} while (found);
							client.name = fName;

							client.connected = true;
							sf::Packet msgPacket;
							msgPacket << static_cast<short>(Protocol::Message) << (client.name + L" connected!");

							sf::Packet playerInfoPacket;
							playerInfoPacket << (short)Protocol::PlayerConnect;
							playerInfoPacket << id;
							playerInfoPacket << client.name;
							for (auto& c : this->clients)
							{
								c.second.socket.send(msgPacket);
								if (id == c.first) continue;

								c.second.socket.send(playerInfoPacket);
							}
						}
					} break;
					case Protocol::RequestChunk:
					{
						int x, y;
						if (inPacket >> x >> y)
						{
							const sf::Vector2i chunk(x, y);
							if (this->chunks.find(chunk) == chunks.end())
							{
								this->chunks.insert(std::make_pair(chunk, server::Chunk(chunk)));
							}
							sf::Packet tp;
							tp << (short)Protocol::ChunkResponse;
							tp << chunks.at(chunk);
							client.socket.send(tp);
						}
					} break;
					}
				}
			}
			else if (status == sf::Socket::Disconnected)
			{
				client.connected = false;
				sf::Packet msgPacket;
				msgPacket << static_cast<short>(Protocol::Message) << (client.name + L" disconnected!");
				sf::Packet dcPacket;
				dcPacket << static_cast<short>(Protocol::PlayerDisconnect) << id;
				for (auto& c : clients)
				{
					c.second.socket.send(msgPacket);
					c.second.socket.send(dcPacket);
				}
				auto it = clients.find(id);
				return;
			}
			else
			{
				//TODO Error
			}
		}
	}

};