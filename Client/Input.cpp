#include "Input.hpp"

void Input::set(int action, std::string string)
{
	char type = string.at(0);
	string.erase(0, 1);
	int id = std::atoi(string.c_str());
	switch (type)
	{
	case 'K':
	{
		this->data[action] = std::static_pointer_cast<BoolInput>(std::make_shared<KeyBoardInput>(static_cast<sf::Keyboard::Key>(id)));
	} break;
	case 'M':
	{
		this->data[action] = std::static_pointer_cast<BoolInput>(std::make_shared<MouseInput>(static_cast<sf::Mouse::Button>(id)));
	} break;
	}
}

bool Input::get(int action)
{
	return this->data[action]->state;
}

void Input::event(sf::Event & e)
{
	switch (e.type)
	{
	case sf::Event::KeyPressed:
	case sf::Event::KeyReleased:
	{
		for (auto& k : this->data)
		{
			const std::shared_ptr<KeyBoardInput> input = std::dynamic_pointer_cast<KeyBoardInput>(k.second);
			if (input != nullptr && input->key == e.key.code)
			{
				input->state = (e.type == sf::Event::KeyPressed);
			}
		}
	} break;
	case sf::Event::MouseButtonPressed:
	case sf::Event::MouseButtonReleased:
	{
		for (auto& k : this->data)
		{
			const std::shared_ptr<MouseInput> input = std::dynamic_pointer_cast<MouseInput>(k.second);
			if (input != nullptr && input->button == e.mouseButton.button)
			{
				input->state = (e.type == sf::Event::MouseButtonPressed);
			}
		}
	} break;
	}
}
