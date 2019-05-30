#include "../System/Error.hpp"

const void Error::Critical(std::string message)
{
	//Globals::client->window.close();
	//TODO linux solution
	bool flag=false;
#if OS == OS_WINDOWS
	//MessageBox(NULL, message.c_str(), "An Error occoured", MB_OK | MB_ICONERROR);
#endif
	std::cout<< message << std::endl;
	exit(0);
	
}
