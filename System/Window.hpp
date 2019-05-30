#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

enum WindowMode
{
    FullScreen,Borderless,Windowed
};

class Window : public sf::RenderWindow
{
private:
    WindowMode currentMode=Borderless;
public:
    const void init(sf::Vector2f resolution,std::string title,WindowMode winMode);
    const WindowMode getWindowMode() const;
};
