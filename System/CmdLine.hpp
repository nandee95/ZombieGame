#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdlib>
#include <functional>
#include "../System/Config.hpp"
#include "../System/Value.hpp"

class CmdLine
{
private:
    std::unordered_map<std::string, std::string> data;
public:
    CmdLine(std::vector<std::string>& parameters);
	void validate(std::string variable, std::function<bool(std::string)> validator);
	//void printRemainingErrors();
    bool exists (std::string key);
    std::string get(std::string key);
};
