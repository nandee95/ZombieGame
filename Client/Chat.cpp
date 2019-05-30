#include "Chat.hpp"

Chat::Chat(sf::Font & font, sf::TcpSocket & server) : font(font), server(server)
{
	inputText.setFont(this->font);
	inputText.setCharacterSize(this->fontSize);
	inputText.setFillColor(this->defaultColor);
	inputText.setOrigin(-this->innerPadding - sf::Vector2f(0, static_cast<float>(shownMessages*fontSize)));
	background.setPrimitiveType(sf::Quads);
	const sf::Vector2f boxMax = sf::Vector2f(static_cast<float>(this->backgroundWidth), static_cast<float>((this->shownMessages + 1)*this->fontSize)) + (2.0f*this->innerPadding);
	background.append(sf::Vertex(sf::Vector2f(0, 0), this->backgroundColor));
	background.append(sf::Vertex(sf::Vector2f(boxMax.x, 0), this->backgroundColor));
	background.append(sf::Vertex(boxMax, this->backgroundColor));
	background.append(sf::Vertex(sf::Vector2f(0, boxMax.y), this->backgroundColor));
}

bool Chat::event(sf::Event& e)
{
	switch (e.type)
	{
	case sf::Event::TextEntered:
	{
		if (open && e.text.unicode != 8 && e.text.unicode != 13)
		{
			this->inputBuffer.insert(cursorpos, 1, static_cast<wchar_t>(e.text.unicode));
			cursorpos++;
			updateInputText();
		}
		if (open) return true;
	} break;
	case sf::Event::KeyPressed:
	{
		switch (e.key.code)
		{
		case sf::Keyboard::Escape:
		{
			if (open)
			{
				open = false;
				this->inputBuffer.clear();
				this->inputText.setString("");
			}
		} break;
		case sf::Keyboard::Return:
		{
			if (open)
			{
				if (!this->inputBuffer.empty())
					this->sendMessage(this->inputBuffer);
				this->inputBuffer.clear();
				this->inputText.setString("");
				cursorpos = 0;
			}
			else
				this->inputText.setString("|");
			open = !open;
		} break;
		case sf::Keyboard::BackSpace:
		{
			if (open)
			{
				if (!this->inputBuffer.empty())
				{
					this->inputBuffer.pop_back();
					cursorpos--;
					updateInputText();
				}
			}
		} break;
		case sf::Keyboard::Left:
		{
			if (open)
			{
				cursorpos = Math::limit(cursorpos - 1, 0, INT_MAX);
				updateInputText();
			}
		} break;
		case sf::Keyboard::Right:
		{
			if (open)
			{
				cursorpos = Math::limit(cursorpos + 1, 0, this->inputBuffer.length());
				updateInputText();
			}
		} break;
		}
		if (open) return true;
	}break;
	}
	return false;
}

void Chat::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(background);
	unsigned int count = 0;
	for (auto it = log.rbegin(); it != log.rend(); it++)
	{
		for (auto& p : *it)
		{
			target.draw(p);
		}
		count++;
		if (count == 5) break;
	}
	if (open) target.draw(this->inputText);
}

void Chat::updateInputText()
{
	std::wstring printed = this->inputBuffer;
	printed.insert(cursorpos, 1, '|');
	this->inputText.setString(printed);
}

void Chat::sendMessage(std::wstring & message)
{
	if (this->onlyWhiteSpace(message)) return;
	sf::Packet packet;
	packet << static_cast<short>(Protocol::Message);
	packet << message;
	this->server.send(packet);
}

void Chat::printMessage(std::wstring message)
{
	sf::Vector2f shift(0, static_cast<float>(this->shownMessages*(this->fontSize - 1)));

	log.push_back(std::vector<sf::Text>());
	std::vector<sf::Text>& data = *std::prev(log.end());

	std::wstring tBuffer;
	std::string cBuffer;

	bool color = false;
	sf::Color currentColor = defaultColor;


	for (auto& c : message)
	{
		if (!color)
		{
			if (c != L'\\')	tBuffer.push_back(c);
			else
			{
				color = true;
			}
		}
		else
		{
			if (c == L'\\')
			{
				cBuffer = "\\\\" + cBuffer;
				tBuffer += std::wstring(cBuffer.begin(), cBuffer.end());
				cBuffer.clear();
				color = false;
				continue;
			}

			if ((c >= L'0' && c <= L'9') || (c >= L'A' && c <= L'F'))
			{
				cBuffer.push_back(c);
				if (cBuffer.length() == 6)
				{
					color = false;
					if (!tBuffer.empty())
					{
						for (auto it = std::next(tBuffer.begin()); it != tBuffer.end(); )
						{
							if (*it == L'\\' && *std::prev(it) == L'\\')
							{
								if (it == std::prev(tBuffer.end()))
								{
									tBuffer.erase(it);
									break;
								}
								it = std::next(tBuffer.erase(it));
							}
							else it++;
						}
						data.push_back(sf::Text(tBuffer, this->font, this->fontSize));
						sf::Text& text = *std::prev(data.end());
						text.setFillColor(currentColor);
						text.setOrigin(-this->innerPadding - shift);
						shift.x += text.getGlobalBounds().width;
					}
					currentColor = Parse::HexColor(cBuffer);
					cBuffer.clear();
					tBuffer.clear();
				}
			}
			else
			{
				cBuffer = "\\" + cBuffer;
				tBuffer += std::wstring(cBuffer.begin(), cBuffer.end());
				cBuffer.clear();
				color = false;
			}
		}
	}
	if (!tBuffer.empty())
	{
		for (auto it = std::next(tBuffer.begin()); it != tBuffer.end(); )
		{
			if (*it == L'\\' && *std::prev(it) == L'\\')
			{
				if (it == std::prev(tBuffer.end()))
				{
					tBuffer.erase(it);
					break;
				}
				it = std::next(tBuffer.erase(it));
			}
			else it++;
		}
		data.push_back(sf::Text(tBuffer, this->font, this->fontSize));
		sf::Text& text = *std::prev(data.end());
		text.setFillColor(currentColor);
		text.setOrigin(-this->innerPadding - shift);
	}

	if (log.size() > maxMessages)
	{
		log.erase(log.begin());
	}

	for (auto& l : log)
	{
		for (auto& t : l)
		{
			const sf::Vector2f o = t.getOrigin();
			t.setOrigin(o.x, o.y + this->fontSize);
		}
	}
}

bool Chat::onlyWhiteSpace(std::wstring & message)
{
	return std::all_of(message.begin(), message.end(), iswspace);
}
