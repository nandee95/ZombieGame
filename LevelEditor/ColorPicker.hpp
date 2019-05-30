#pragma once
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>

//#include "../System/Globals.hpp"
#include "../System/Math.hpp"

class LevelEditor;
class Tool;

class ColorPicker;

class ColorPickerWindow //Leveleditor only holds one copy
{
private:
    ColorPicker* colorPicker;
    //Heavies
    tgui::ChildWindow::Ptr window;
    sf::Shader shader;
    sf::RectangleShape indicatorHue;
    sf::CircleShape indicatorSV;
	//Holders
    bool holdOutside = false;
    bool holdInside = false;
    //Color values
    sf::Vector3f rgb; //0-1 0-1 0-1
    sf::Vector3f hsv; //0-360 0-1 0-1
    float alpha=1.0f; //0-1
    //Rectangle
    const float rectMin=65.1474f;
    const float rectMax=234.8526f;
    const float rectSide=169.7052f;
    //Update
    void update();
public:
    ColorPickerWindow(tgui::Gui& gui);
    void event(sf::Event& e);
    void show(ColorPicker* colorPicker);
};

class ColorPicker
{
private:
	sf::Color color=sf::Color::White;
	tgui::Panel::Ptr panel;
	void create();
public:
	ColorPicker();
	ColorPicker(sf::Color color);
	sf::Color getColor();
	void setColor(sf::Color color);
	tgui::Panel::Ptr getWidget();
};
