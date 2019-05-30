#include "../System/CfgFile.hpp"
#include <algorithm>

CfgFile::CfgFile()
{

}

const void CfgFile::setFilename(std::string filename)
{
	this->filename = filename;
}

const std::string & CfgFile::getFilename() const
{
	return this->filename;
}

CfgFile::Value CfgFile::getValue(std::string group, std::string variable)
{
	return Value(this->data.at(group).at(variable).value);
}

bool CfgFile::read()
{
	std::ifstream fs(this->filename);
	if (!fs.good())
	{
		return false; // Failed to open file
	}
	std::string currentGroup = "";

	const std::regex group("[ \\t]*\\[[ \\t]*([A-Za-z0-9_]*)[ \\t]*\\][ \\t]*");
	const std::regex value("[\\t| ]*([A-Za-z0-9_]*)[\\t| ]*=[\\t| ]*(?:([\"']?)((?:\\\\?.)*?)\\2)");

	std::string line;
	while (std::getline(fs, line))
	{
		//Skip lines that contains only whitespace
		if (std::all_of(line.begin(), line.end(), isspace)) continue;

		std::smatch matches;

		//Match variables
		if (!currentGroup.empty() && std::regex_match(line, matches, value) && matches.ready())
		{
			this->data[currentGroup].insert(std::make_pair(matches[1], Data(matches[3])));
		} //Match groups
		else if (std::regex_match(line, matches, group) && matches.ready())
		{
			currentGroup = matches[1];
		}
	}
	return true;
}


void CfgFile::validate(std::string group, std::string variable, std::string defaultValue, std::function<bool(std::string)> validator)
{
	if (this->data.find(group) == this->data.end() || // Group not exists
		this->data[group].find(variable) == this->data[group].end()) // OR Variable not exists
	{
		//Insert it with its default value
		this->data[group].insert(std::make_pair(variable, Data(defaultValue, true)));
		return;
	}
	Data &var = this->data.at(group).at(variable);
	if (!validator(var.value)) //If the presented data not valid
	{
		//Roll back to default value
		var.value = defaultValue;
	}
	var.validated = true;
}
void CfgFile::cleanUp()
{
	for (auto it = this->data.begin(); it != this->data.end();) //Remove records that didnt pass the validation process
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end();)
		{
			if (!it2->second.validated) it2 = it->second.erase(it2);
			else it2++;
		}
		if (it->second.empty()) // Remove the group if it became empty
		{
			it = this->data.erase(it);
		}
		else it++;
	}
}

bool CfgFile::update()
{
	std::ofstream fs(this->filename, std::ofstream::trunc);
	if (!fs.good()) return false; //Failed to open file
	for (auto& g : this->data)
	{
		fs << "[" << g.first << "]" << std::endl;
		for (auto& v : g.second)
		{
			//Begins or ends with whitespace
			if(v.second.value.at(0)==' ' || v.second.value.at(v.second.value.length()-1) == ' ' ||
				v.second.value.at(0) == '\t' || v.second.value.at(v.second.value.length() - 1) == '\t' ) //Starts or ends with whitespace
				fs << v.first << "=\"" << v.second.value << "\"" << std::endl;
			else
				fs << v.first << "=" << v.second.value << std::endl;
		}
	}

	return true;
}

CfgFile::CfgFile(std::string filename)
{
	this->filename = filename;
}

CfgFile::Data::Data(std::string _value, bool _validated) { this->value = _value; this->validated = _validated; }

sf::Vector2i CfgFile::Value::asVec2i()
{
	return Parse::Vec2i(string);
}

bool CfgFile::Value::asBool()
{
	return Parse::Bool(string);
}

int CfgFile::Value::asInt()
{
	return Parse::Int(this->string);
}

CfgFile::Value::Value(std::string string) : string(string) {}
