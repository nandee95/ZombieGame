#pragma once
#include "../System/State.hpp"
#include "../System/ResourceManager.hpp"
#include "../System/Transitions.hpp"
#include "../Entities/Cursor.hpp"
#include "../Menu/Settings.hpp"
#include <array>

class Menu;

enum MenuItems : char
{
	ItemCreateGame,
	ItemJoinGame,
	ItemSettings,
	ItemCredits,
	ItemExit,
	___Size
};

class MenuItem : public sf::Text
{
private:
	const sf::Time mouseInAnimationTime = sf::milliseconds(250);
	const sf::Time mouseOutAnimatonTime = sf::milliseconds(250);
protected:
	friend Menu;
	sf::Vector2f basePos;
	int seed;
	MenuItems id;
	bool mouseHovers = false;
public:
	MenuItem();

	void update(sf::Time& dt);

	void clicked();

	void mouseIn(sf::Time& time);

	void mouseOut();
};

class Menu : public State
{
private:
	const std::array<std::string, MenuItems::___Size> menuItemTexts={
		"Create game",
		"Join game",
		"Settings",
		"Credits",
		"Exit"
	};
	const float spacing = 70.0f;
	const std::string menuFontFile = "fonts/mainmenu.ttf";
	ResourceManager resMg;
	sf::Font& menuFont;
	std::array<MenuItem, MenuItems::___Size> texts;
	sf::Clock timer;
	bool fadeIn = true;
	bool fadeOut = false;
	sf::Time actionStart;
	sf::SoundBuffer& mouseInBeep;
	sf::Sound mouseInBeepSound;
	Cursor cursor;
	sf::Time lastFrame;
public:
	Menu(sf::RenderWindow& window);

	virtual void event(sf::Event& e);
	virtual void update();
	virtual void draw(sf::RenderTarget& target);
};