#pragma once
#include <string>
#include <sstream>
#include <regex>
#include <functional>

#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

//TODO check consts and references....

namespace Validate
{
    bool Resolution(sf::Vector2i resolution);

	//String based approaches:

	bool Any(std::string& str);
	std::function<bool(std::string)> String(std::string str1);
	bool Uint(std::string& str);
	bool Int(std::string str);
	bool Bool(std::string str);
	//0-255.0-255.0-255.0-255
	bool Ipv4Address(std::string str);
	//0-255.0-255.0-255.0-255:0-65535
	bool Ipv4AddressPort(std::string str);
	std::function<bool(std::string)> Regex(std::string regex);
	//min <= value <= max
	std::function<bool(std::string)> IntRange(int min, int max);
	bool Vec2i(std::string str);
};

namespace Parse
{
	bool Bool(std::string str);
	long long LongLong(std::string str);
	long Long(std::string str);
	unsigned int UInt(std::string str);
	int Int(std::string str);
	float Float(std::string str);
	double Double(std::string str);
	sf::Vector2i Vec2i(std::string str);
	sf::IpAddress Ipv4Address(std::string str);
	sf::Color HexColor(std::string str);
};