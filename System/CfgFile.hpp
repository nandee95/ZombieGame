#pragma once
#include <fstream>
#include <unordered_map>
#include <regex>
#include <functional>
#include <SFML/System/Vector2.hpp>
#include "../System/Value.hpp"

class CfgFile
{
private:
	struct Data
	{
		std::string value;
		bool validated = false;
		Data(std::string _value, bool _validated = false);
	};
	struct Value
	{
		const std::string string;
		sf::Vector2i asVec2i();
		bool asBool();
		int asInt();
		Value(std::string string);
	};
	std::unordered_map<std::string, std::unordered_map<std::string, Data>> data;
	std::string filename;
public:
	CfgFile(std::string filename);
	CfgFile();
	/*
	Sets the file used by the class
	*/
	const void setFilename(std::string filename);
	/*
	Returns the filename used by the classs
	*/
	const std::string& getFilename() const;
	/*
	Returns a value from the stored data
	Warning: You should only query variables that were validated before!
	*/
	Value getValue(std::string group, std::string variable);
	/*
	Reads the data from the file
	*/
	bool read();
	/*
	Validates every data readed from the file, and makes sure every necessary data exists in our data structure.
	*/
	void validate(std::string group, std::string variable, std::string defaultValue, std::function<bool(std::string)> validator);
	/*
	Removes the data that didnt maintoned by the validation process...
	*/
	void cleanUp();
	/*
	Writes valid data right back to the target file
	*/
	bool update();
};
