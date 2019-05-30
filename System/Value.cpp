#include "../System/Value.hpp"
#include <iostream>


bool Validate::Any(std::string &str)
{
	return true;
}

std::function<bool(std::string)> Validate::String(std::string str1)
{
	return [str1](std::string str2) { return str1 == str2; };
}

bool Validate::Uint(std::string &str)
{
	return std::all_of(str.begin(), str.end(), isdigit);
}

bool Validate::Int(std::string str)
{
	if (str.at(0) == '-') str.erase(0, 1);
	return Uint(str);
}

bool Validate::Bool(std::string str)
{
	if (str == "0" || str == "1") return true;
	std::for_each(str.begin(), str.end(), [](char& ch)
	{
		ch = std::tolower(ch, std::locale());
	});
	return str == "true" || str == "false";
}

bool Validate::Ipv4Address(std::string str)
{
	if (str == "localhost")
	{
		return true;
	}
	const std::regex ipv4("(?:(?:1?[0-9]{1,2}|2[0-4][0-9]|25[0-5])\\.){3}(?:1?[0-9]{1,2}|2[0-4][0-9]|25[0-5])");
	return std::regex_match(str, std::smatch(), ipv4);
}

bool Validate::Ipv4AddressPort(std::string str)
{
	std::string::size_type pos = str.find("localhost");
	if (pos != std::string::npos) {
		str.replace(pos, 9, "127.0.0.1");
	}
	const std::regex ipv4port("((?:(?:1?[0-9]{1,2}|2[0-4][0-9]|25[0-5])\\.){3}(?:1?[0-9]{1,2}|2[0-4][0-9]|25[0-5])):([0-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])");
	return std::regex_match(str, std::smatch(), ipv4port);
}

std::function<bool(std::string)> Validate::Regex(std::string regex)
{
	return [regex](std::string str) {
		return std::regex_match(str, std::smatch(), std::regex(regex));
	};
}

std::function<bool(std::string)> Validate::IntRange(int min, int max)
{
	return [min, max](std::string str) {
		const int converted = std::atoi(str.c_str());
		return (Validate::Int(str) && min <= converted && max >= converted);
	};
}

bool Validate::Vec2i(std::string str)
{
	const std::string::size_type pos = str.find_first_of(';');
	if (pos == std::string::npos) return false;
	const std::string first = str.substr(0, pos);
	const std::string second = str.substr(pos + 1, str.length() - pos - 1);
	return Int(first) && Int(second);
}

bool Validate::Resolution(sf::Vector2i resolution)
{
    std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
    for (std::size_t i = 0; i < modes.size(); ++i)
    {
        if (modes[i].width < 800 && modes[i].height < 600) continue;
        if (resolution.x == modes[i].width && resolution.y == modes[i].height) return true;
    }
    return false;
}

bool Parse::Bool(std::string str) //Need copy here
{
	if (str == "1") return true;
	if (str == "0") return false;
	std::for_each(str.begin(), str.end(), [](char& ch)
	{
		ch = std::tolower(ch, std::locale());
	});
	return str == "true";
}

long long Parse::LongLong(std::string str)
{
	return std::atoll(str.c_str());
}

long Parse::Long(std::string str)
{
	return std::atol(str.c_str());
}

unsigned int Parse::UInt(std::string str)
{
	return static_cast<unsigned int>(Parse::Int(str));
}

int Parse::Int(std::string str)
{
	return std::atoi(str.c_str());
}

float Parse::Float(std::string str)
{
	return static_cast<float>(std::atof(str.c_str()));
}

double Parse::Double(std::string str)
{
	return std::atof(str.c_str());
}

sf::Vector2i Parse::Vec2i(std::string str)
{
	const std::string::size_type pos = str.find_first_of(';');
	std::string first = str.substr(0, pos);
	std::string second = str.substr(pos + 1, str.length() - pos - 1);
	return sf::Vector2i(Int(first), Int(second));
}

sf::IpAddress Parse::Ipv4Address(std::string str)
{
	return sf::IpAddress(str);
}

sf::Color Parse::HexColor(std::string str)
{
	sf::Color c;
	unsigned int buffer;
	std::stringstream ss;
	ss << std::hex << str.substr(0, 2);
	ss >> buffer;
	c.r = static_cast<char>(buffer);
	ss.clear();
	ss << std::hex << str.substr(2, 2);
	ss >> buffer;
	c.g = static_cast<char>(buffer);
	ss.clear();
	ss << std::hex << str.substr(4, 2);
	ss >> buffer;
	c.b = static_cast<char>(buffer);
	return c;
}
