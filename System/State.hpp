#pragma once
#include <SFML/Graphics.hpp>

class State
{
public:
    virtual void event(sf::Event& e) = 0;
    virtual void update() = 0;
    virtual void draw(sf::RenderTarget& target) = 0;
};
