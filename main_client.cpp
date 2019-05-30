#include "System/Config.hpp"
#include "Client/Manager.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <filesystem>

namespace fs = std::experimental::filesystem;


#if OS == OS_WINDOWS
#include <windows.h> //Entry point
#include <stdlib.h> //__argc,__argv
#endif // OS


int main(int argc, char *argv[])
{
	std::vector<std::string> parameters;
	for (int i = 0; i < argc; i++)
	{
		parameters.push_back(argv[i]);
	}
#if OS == OS_WINDOWS
	//Change working directory to the parent directory
	_wchdir(fs::current_path().parent_path().c_str());
#endif // OS
	std::shared_ptr<Manager> game = std::make_shared<Manager>(parameters);
	return 	game->run();
	return 0;
}