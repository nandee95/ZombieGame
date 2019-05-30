#define SERVER

#include "System/Config.hpp"
#include <iostream>
#include "Server/Server.hpp"
#include <filesystem>

namespace fs = std::experimental::filesystem;


int main(int argc, char *argv[])
{
#if OS == OS_WINDOWS
	//Change working directory to the parent directory
	_wchdir(fs::current_path().parent_path().c_str());
#endif // OS

	CfgFile cfg("../server.ini");
	cfg.read();

	cfg.validate("Server", "Port", "5000", Validate::IntRange(0, 65535));

	cfg.cleanUp();
	cfg.update();
	std::unique_ptr<Server> server;
	server = std::make_unique<Server>(cfg);
	if (!server->isRunning()) return 1;
	while (server->isRunning())
	{
		sf::sleep(sf::milliseconds(10));
	}
	return 	0;
}