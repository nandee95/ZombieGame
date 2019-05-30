#include "../Client/Manager.hpp"

//Initialization
Manager::Manager(std::vector<std::string>& parameters)
{
    const std::string title = "Zombie Game - Version " + std::to_string(VERSION_MAJOR) + "." + std::to_string(VERSION_MINOR) + " - Build " + std::to_string(BUILD_NO);
    //Default settings
	const sf::VideoMode& desktopMode = sf::VideoMode().getDesktopMode();

	CfgFile cfg("config.ini");
	cfg.read();
	cfg.validate("Video", "Vsync", "True", Validate::Bool);
	cfg.validate("Video", "Mode", "1", Validate::IntRange(0, 2)); //Borderless
	cfg.validate("Video", "FpsLimit", "60", Validate::Int); //Borderless
	cfg.validate("Video", "Resolution", std::to_string(desktopMode.width) + ";" + std::to_string(desktopMode.height), [](std::string str) {
		return Validate::Vec2i(str) && Validate::Resolution(Parse::Vec2i(str)) ;
	});

	cfg.validate("InternalServer", "Port", "5000", Validate::IntRange(0, 65535));
	cfg.validate("InternalServer", "Enabled", "True", Validate::Bool);
	cfg.validate("Client", "Ip", "localhost", Validate::Ipv4Address);
	cfg.validate("Client", "Port", "5000", Validate::IntRange(0, 65535));

	cfg.validate("Gameplay", "Name", "Player", Validate::Any);

	cfg.validate("Controls", "MoveUp", "K"+std::to_string(sf::Keyboard::W), Validate::Regex("[K|M][0-9]{1,3}"));
	cfg.validate("Controls", "MoveDown", "K" + std::to_string(sf::Keyboard::S), Validate::Regex("[K|M][0-9]{1,3}"));
	cfg.validate("Controls", "MoveLeft", "K" + std::to_string(sf::Keyboard::A), Validate::Regex("[K|M][0-9]{1,3}"));
	cfg.validate("Controls", "MoveRight", "K" + std::to_string(sf::Keyboard::D), Validate::Regex("[K|M][0-9]{1,3}"));
	cfg.validate("Controls", "Run", "K" + std::to_string(sf::Keyboard::LShift), Validate::Regex("[K|M][0-9]{1,3}"));
	cfg.validate("Controls", "InventoryToggle", "K" + std::to_string(sf::Keyboard::I), Validate::Regex("[K|M][0-9]{1,3}"));

	cfg.cleanUp();
	cfg.update();

	sf::Vector2f resolution = sf::Vector2f(cfg.getValue("Video", "Resolution").asVec2i());
	WindowMode mode = static_cast<WindowMode>(std::atoi(cfg.getValue("Video","Mode").string.c_str()));
	bool vsync = cfg.getValue("Video", "Vsync").asBool();

    //TODO: Command line overrides
    this->window.init(resolution,title,mode);
	this->window.setVerticalSyncEnabled(vsync);
	window.setFramerateLimit(cfg.getValue("Video", "FpsLimit").asInt());

	this->setGameState(std::make_shared<SinglePlayerGame>(this->window,cfg));
	//this->setGameState(std::make_shared<Menu>(window));
}

//Main thread's loop
int Manager::run()
{
    while(this->window.isOpen())
    {
        sf::Event e;
        while(this->window.pollEvent(e))
        {
            if(e.type == sf::Event::Closed)
            {
				break;
            }
            state->event(e);
        }

        this->window.clear(sf::Color::White);
        this->state->update();
        this->state->draw(this->window);
        this->window.display();
    }
    return EXIT_OK;
}


void Manager::setGameState(std::shared_ptr<State> state)
{
	this->state = state;
}
