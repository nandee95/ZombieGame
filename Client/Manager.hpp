#pragma once
#include <string>
#include <memory>

#include <SFML/Graphics.hpp>

#include "../System/CmdLine.hpp"
#include "../System/CfgFile.hpp"
#include "../System/Build.hpp"
#include "../System/Window.hpp"
#include "../System/Value.hpp"
#include "../System/State.hpp"
#include "../Client/SinglePlayerGame.hpp"
#include "../Client/Input.hpp"
#include "../Client/Player.hpp"
#include "../Menu/Menu.hpp"

class Manager
{
private:
    Window window;
    std::shared_ptr<State> state;

public:
    Manager(std::vector<std::string>& parameters);
	void setGameState(std::shared_ptr<State> state);
    int run();
};
