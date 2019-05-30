#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <unordered_map>
#include <regex>
#include "../Server/Protocol.hpp"
#include <iostream>
#include "../System/Math.hpp"
#include "../System/Value.hpp"

class Chat : public sf::Drawable, sf::Transformable
{
private:
	const unsigned int maxMessages = 100;
	const int shownMessages = 5;
	const int fontSize = 20;
	const sf::Color defaultColor = sf::Color(220, 220, 220, 255);
	const sf::Color backgroundColor = sf::Color(0, 0, 0, 127);
	const unsigned int backgroundWidth = 500;
	const sf::Vector2f innerPadding = sf::Vector2f(10.0f, 10.0f);
	int cursorpos = 0;

	sf::Font& font;
	sf::TcpSocket& server;

	sf::Text inputText;
	std::wstring inputBuffer;
	std::vector<std::vector<sf::Text>> log;
	bool open = false;
	sf::VertexArray background;
public:
	Chat(sf::Font& font, sf::TcpSocket& server);

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void updateInputText();

	bool event(sf::Event& e);

	void sendMessage(std::wstring& message);

	void printMessage(std::wstring message);
private:

	static bool onlyWhiteSpace(std::wstring& message);
};