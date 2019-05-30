#include "Menu.hpp"

MenuItem::MenuItem()
{
	seed = Math::random(0, 20000);
	this->setOutlineColor(sf::Color::White);
	this->setColor(sf::Color::Red);
}

void MenuItem::update(sf::Time & dt)
{
	const float t = this->getOutlineThickness();
	if (mouseHovers && t != 3.0f)
	{
		const float progress = static_cast<float>(dt.asMicroseconds()) / 1000000.0f;
		this->setOutlineThickness(Math::limit(t + progress*15.0f, 0, 3));
	}
	else if (!mouseHovers && t != 0.0f)
	{
		const float progress = static_cast<float>(dt.asMicroseconds()) / 1000000.0f;
		this->setOutlineThickness(Math::limit(t - progress*15.0f, 0, 3));
	}
}

void MenuItem::clicked()
{
	switch (id)
	{
	case MenuItems::ItemCreateGame:
	{

	} break;
	case MenuItems::ItemExit:
	{
		exit(0);
	} break;
	}
}

void MenuItem::mouseIn(sf::Time & time)
{
}

void MenuItem::mouseOut()
{
}

Menu::Menu(sf::RenderWindow & window)
	: menuFont(resMg.addFont(menuFontFile)),
	mouseInBeep(resMg.addSound("sound/menu/item_mousein.ogg")), mouseInBeepSound(mouseInBeep),
	cursor("gui/menu/cursor.png", 0.4f)
{
	window.setMouseCursorVisible(false);
	sf::Vector2f pos(100, 100);
	for (int i = 0; i < MenuItems::___Size; i++)
	{
		MenuItem& item = texts.at(i);
		item.setString(menuItemTexts.at(i));
		item.setPosition(pos);
		item.setFont(menuFont);
		item.basePos = pos;
		item.setScale(1.5f, 1.5f);
		item.id = static_cast<MenuItems>(i);
		pos.y += spacing;
	}
}

void Menu::event(sf::Event & e)
{
	switch (e.type)
	{
	case sf::Event::MouseButtonPressed:
	{
		if (e.mouseButton.button == sf::Mouse::Left)
		{
			for (auto& t : this->texts)
			{
				if (t.getGlobalBounds().contains(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)))
				{
					t.clicked();
					break;
				}
			}
		}
	} break;
	case sf::Event::MouseMoved:
	{
		for (auto& t : this->texts)
		{
			if (t.getGlobalBounds().contains(sf::Vector2f(e.mouseMove.x, e.mouseMove.y)))
			{
				if (!t.mouseHovers)
				{
					t.mouseHovers = true;
					t.mouseIn(timer.getElapsedTime());
					mouseInBeepSound.play();
					cursor.setPointer(Cursor::Point);
				}
			}
			else if (t.mouseHovers)
			{
				t.mouseHovers = false;
				t.mouseOut();
				cursor.setPointer(Cursor::Normal);
			}
		}
		cursor.update(sf::Vector2f(e.mouseMove.x, e.mouseMove.y));
	} break;
	}
}

void Menu::update()
{
	sf::Time& time = timer.getElapsedTime();
	sf::Time dt = time - lastFrame;
	for (auto& t : this->texts)
	{
		if (fadeIn)
		{
			float progress = static_cast<float>((timer.getElapsedTime() - actionStart).asMicroseconds()) / 1000000.0f;
			sf::Vector2f shift = sf::Vector2f(
				Math::noise(static_cast<float>(timer.getElapsedTime().asMilliseconds() + t.seed) / 5000.0f)*35.0f, 0);
			t.setPosition(Transition::bezierQuadratic(progress, 0, 1, 1)*400.0f - 300.0f + shift.x, t.basePos.y);
			const sf::Color& c = t.getFillColor();
			t.setFillColor(sf::Color(c.r, c.g, c.b, Math::limit(progress, 0, 1)*255.0f));
			if (progress >= 1.0f)
			{
				fadeIn = false;
				const sf::Color& c = t.getFillColor();
				t.setFillColor(sf::Color(c.r, c.g, c.b, 255.0f));

			}
		}
		else
		{
			t.setPosition(t.basePos + sf::Vector2f(
				Math::noise(static_cast<float>(timer.getElapsedTime().asMilliseconds() + t.seed) / 5000.0f)*35.0f, 0));
			t.update(dt);
		}

	}
	lastFrame = time;
}

void Menu::draw(sf::RenderTarget & target)
{
	target.clear(sf::Color::Black);
	for (auto& t : this->texts)
	{
		target.draw(t);
	}
	target.draw(cursor);
}
