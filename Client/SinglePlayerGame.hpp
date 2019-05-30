#include "../System/State.hpp"
#include "../System/CfgFile.hpp"
#include "../Client/Client.hpp"

#include "../Server/Server.hpp"

class SinglePlayerGame : public State
{
private:
	std::unique_ptr<Server> server;
	std::unique_ptr<Client> client;
public:
	SinglePlayerGame(Window& window, CfgFile& cfg);
	virtual void event(sf::Event& e);
	virtual void update();
	virtual void draw(sf::RenderTarget& target);
};