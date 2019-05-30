#pragma once
#include "../LevelEditor/ToolBase.hpp"
#include "../LevelEditor/ColorPicker.hpp"
#include <TGUI/TGUI.hpp>
#include <iostream> //TODO remove
#include <string>
#include <experimental/filesystem>
#include <iomanip>//setprecision

namespace fs = std::experimental::filesystem;
/*
Modes:
Left Click: select/transform
Middle click: move canvas
Right Click: place

Attributes:
Origin
Position
Rotation
Scale(X)
Repeat

Snap to grid(SHIFT)
Properties(Space)

*/

class ToolSprite : public Tool
{
private:
	//window
	struct Thumbnail
	{
		sf::Texture texture;
		sf::Vector2u originalResolution;
		Thumbnail(sf::Texture _texture,sf::Vector2u _resolution)
		{	texture = _texture;	originalResolution = _resolution;	}
	};
	int prevScrollValue = 0;
	const std::string folder = "sprites";
    tgui::ChildWindow::Ptr toolWindow;
	sf::Texture folderIcon;
	sf::Texture folderUpIcon;
	float distance = 0.0f;

	bool mirroredX = false;
	bool mirroredY = false;
	sf::Vector2f originNorm=sf::Vector2f(0.5f,0.5f);

	std::string selectedFilename;
	std::shared_ptr<sf::Texture> ghostTexture;
	std::shared_ptr<sf::Sprite> ghostSprite;
	std::unordered_map<std::string, Thumbnail> thumbnails;
	//ColorPicker
    ColorPicker colorPicker;

	//Mouse
    bool snapToGrid = false;
    bool placeHold = false;

	//Funcs
	void setSelectedSprite(std::string filename);
    void spawnSprite(sf::Vector2f screenPos);
	std::string getFileSize(unsigned int size);
	void setFilePanelDirectory(std::string directory, int level=0,bool init=false);
    Thumbnail makeThumbnail(std::string filename,sf::Vector2f targetSize);
    bool isFile(std::string filename);
protected:
    virtual void init_gui(tgui::Gui& gui);
    virtual void draw(sf::RenderTarget& target);
    virtual void event(sf::Event& e);
    virtual void selected();
    virtual void unselected();
public:
    ToolSprite();
};
