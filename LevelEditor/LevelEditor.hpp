#pragma once
#include <functional>

#include <TGUI/TGUI.hpp>

#include "../System/State.hpp"
#include "../System/Error.hpp"
#include "../System/Math.hpp"
#include "../LevelEditor/Cursor.hpp"
#include "../LevelEditor/ToolBase.hpp"
#include "../LevelEditor/ToolTile.hpp"
#include "../System/ResourceManager.hpp"
#include "../Client/Level.hpp"

class ColorPickerWindow;
class ColorPicker;

class LevelEditor : public State
{
private:
    const int zoomValue[7]= {25,33,50,67,100,150,200};
    int zoom=4;

    //Controllers
    bool isGrabbed=false;
    sf::Vector2f oldPosition;

    //Axes
    bool showAxes = true;

    //screenToWorld
    sf::Vector2f swConvert;


    //GUI
    tgui::Gui gui;
    tgui::MenuBar::Ptr menuBar;
    tgui::Label::Ptr zoomLabel;
	tgui::Label::Ptr gridLabel;

    //Tools
    std::unique_ptr<Tool> tools[1] =
    {
        std::make_unique<ToolTile>()
    };
    unsigned int currentTool = 0;
public:
	//Level
	Level level;
	//View
	sf::View view;
	//Data
	ResourceManager resourceManager;
	//Cursor
	std::unique_ptr<Cursor> cursor;
	//Colorpicker
    std::shared_ptr<ColorPickerWindow> colorPickerWindow;
	void resetView();
    LevelEditor();
    ~LevelEditor();
protected:
    //Pure virtuals from Base class
    virtual void event(sf::Event& e);
    virtual void update();
    virtual void draw(sf::RenderTarget& target);

    //initializationó
    void init_gui();

    //functions
    void setZoom(int zoom);
    int getZoomValue();
public:
    sf::Vector2f screenToWorldScale(sf::Vector2f screen);
    sf::Vector2f screenToWorldPosition(sf::Vector2f screen);
};
