#pragma once
#include <TGUI/TGUI.hpp>

class LevelEditor;

class Tool
{
public:
    static LevelEditor* levelEditor;
    std::string name="";
    virtual void init_gui(tgui::Gui& gui) = 0;
    virtual void draw(sf::RenderTarget& target) = 0;
    virtual void event(sf::Event& e) = 0;
    virtual void selected() = 0;
    virtual void unselected() = 0;
};
