#include "SinglePlayerGame.hpp"

SinglePlayerGame::SinglePlayerGame(Window & window, CfgFile & cfg)
{
	if (cfg.getValue("InternalServer", "Enabled").asBool())
	{
		server = std::make_unique<Server>(cfg);
		if (!server->isReady())
		{
			server.release();
		}
	}
	client = std::make_unique<Client>(window, cfg);
}

void SinglePlayerGame::event(sf::Event & e)
{
	this->client->event(e);
}

void SinglePlayerGame::update()
{
	this->client->update();
}

void SinglePlayerGame::draw(sf::RenderTarget & target)
{
	this->client->draw(target);
}
