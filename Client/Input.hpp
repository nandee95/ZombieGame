#pragma once
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Event.hpp>
#include <unordered_map>
#include <memory>
#include <string>

class BoolInput
{
public:
	virtual ~BoolInput() {};
	bool state = false;
};

class Input;

class KeyBoardInput : public BoolInput
{
protected:
	friend Input;
	sf::Keyboard::Key key;
public:
	KeyBoardInput(sf::Keyboard::Key key) : key(key) {}
};

class MouseInput : public BoolInput
{
protected:
	friend Input;
	sf::Mouse::Button button;
public:
	MouseInput(sf::Mouse::Button button) : button(button) {}
};

class Input
{
private:
	std::unordered_map<int, std::shared_ptr<BoolInput>> data;
public:

	void set(int action, std::string string);

	bool get(int action);

	void event(sf::Event& e);
};