#include "System/Config.hpp"
#include "Client/Manager.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>

#include "LevelEditor/LevelEditor.hpp"

#if OS == OS_WINDOWS
#include <windows.h> //Entry point
#include <stdlib.h> //__argc,__argv
#endif // OS

#if FALSE //OS == OS_WINDOWS
int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    std::shared_ptr<GameManager> game=std::make_shared<GameManager>(__argc,__argv);
#else
//TODO correct linux entry point without consle window
int main(int argc, char *argv[])
{
	//std::shared_ptr<GameManager> game = std::make_shared<GameManager>(argc, argv);
#endif // OS
	//game->setGameState(std::static_pointer_cast<GameState>(std::make_shared<LevelEditor>()));
	return 0;//game->run();
}