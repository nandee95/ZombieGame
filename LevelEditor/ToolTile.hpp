#pragma once
#include "../LevelEditor/ToolBase.hpp"
#include "../LevelEditor/ColorPicker.hpp"
#include <TGUI/TGUI.hpp>
#include <iostream> //TODO remove
#include <string>
#include <experimental/filesystem>
#include <iomanip>//setprecision
#include <thread>
namespace fs = std::experimental::filesystem;

class ToolTile : public Tool
{
private:
	//window
	struct Thumbnail
	{
		tgui::Texture texture;
		sf::Vector2u originalResolution;
		Thumbnail(sf::Texture _texture, sf::Vector2u _resolution)
		{
			texture = _texture;	originalResolution = _resolution;
		}
	};
	int prevScrollValue = 0;
	const std::string folder = "tiles";
	tgui::ChildWindow::Ptr toolWindow;
	std::string directory;
	sf::Texture folderIcon;
	sf::Texture folderUpIcon;
	int cursorPos = 0;

	std::string selectedFilename;
	std::shared_ptr<sf::Texture> ghostTexture;
	std::shared_ptr<sf::Sprite> ghostSprite;
	std::unordered_map<std::string, Thumbnail> thumbnails;

	//Mouse
	bool snapToGrid = false;
	bool placeHold = false;

	//Funcs
	void setSelectedSprite(std::string filename);
	std::string getFileSize(unsigned int size);
	void setFilePanelDirectory(std::string directory, int level = 0, bool init = false);
	Thumbnail makeThumbnail(std::string filename, sf::Vector2f targetSize);
	bool isFile(std::string filename);
protected:
	virtual void init_gui(tgui::Gui& gui);
	virtual void draw(sf::RenderTarget& target);
	virtual void event(sf::Event& e);
	virtual void selected();
	virtual void unselected();
public:
	ToolTile();
};
