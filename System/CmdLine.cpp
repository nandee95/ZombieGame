#include "../System/CmdLine.hpp"

CmdLine::CmdLine(std::vector<std::string>& parameters)
{
	std::string variable;
	for (auto& p : parameters)
	{
		if (p.at(0) == '-') //Its a variable
		{
			variable = p;
			continue;
		}
		if (variable != "")
		{
			if(!this->exists(variable))
				this->data[variable] = p;
			else
				this->data[variable] += " " + p;
		}
	}
}

void CmdLine::validate(std::string variable,std::function<bool(std::string)> validator)
{
	if (!this->exists(variable)) return;


}


bool CmdLine::exists(std::string key)
{
    return this->data.find(key) != this->data.end();
}

std::string CmdLine::get(std::string key)
{
    return this->data.at(key);
}
