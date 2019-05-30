#include "../System/Window.hpp"

const void Window::init(sf::Vector2f resolution,std::string title,WindowMode winMode)
{
    this->currentMode = winMode;
    sf::ContextSettings cs;
    cs.antialiasingLevel = 8;
    switch(winMode)
    {
    case FullScreen:
    {
        this->create(sf::VideoMode(static_cast<int>(resolution.x),static_cast<int>(resolution.y)), title.c_str(), sf::Style::Fullscreen);
    }
    break;
    case Borderless:
    {
        this->sf::RenderWindow::create(sf::VideoMode().getDesktopMode(), title.c_str(), sf::Style::None);
    }
    break;
    case Windowed:
    {
        this->sf::RenderWindow::create(sf::VideoMode(static_cast<int>(resolution.x), static_cast<int>(resolution.y)), title.c_str(), sf::Style::Close);
    }
    break;
    }
}

const WindowMode Window::getWindowMode() const
{
    return this->currentMode;
}
