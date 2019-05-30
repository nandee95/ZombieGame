#include "../LevelEditor/ToolTile.hpp"
#include "../LevelEditor/LevelEditor.hpp"

ToolTile::ToolTile()
{
	this->name = "Tile place tool";
}

void ToolTile::init_gui(tgui::Gui& gui)
{
	this->toolWindow = tgui::ChildWindow::create();
	this->toolWindow->setSize(320, 900);
	this->toolWindow->setPosition(40, 60);
	this->toolWindow->setTitle(this->name);
	this->toolWindow->connect("Closed", [&]()
	{
		this->toolWindow->hide();
	});
	this->toolWindow->keepInParent();
	this->toolWindow->hide();
	gui.add(this->toolWindow);

	auto filePanel = tgui::Panel::create();
	filePanel->setSize(270, 600);
	filePanel->setPosition(10, 290);

	this->toolWindow->add(filePanel, "filePanel");

	auto fileScrollBar = tgui::Scrollbar::create();
	fileScrollBar->setSize(30, filePanel->getSize().y);
	fileScrollBar->setPosition(filePanel->getPosition().x + filePanel->getSize().x, filePanel->getPosition().y);
	fileScrollBar->setArrowScrollAmount(65);
	fileScrollBar->setLowValue(static_cast<unsigned int>(filePanel->getSize().y));
	fileScrollBar->connect("ValueChanged", [this](tgui::Panel::Ptr panel, int newValue) {
		const int dist = this->prevScrollValue - newValue;
		for (auto& widget : panel->getWidgets())
		{
			widget->setPosition(widget->getPosition().x, widget->getPosition().y + dist);
		}
		this->prevScrollValue = newValue;
	}, filePanel);
	this->toolWindow->add(fileScrollBar, "fileScrollBar");

	int count = 0;

	folderIcon.loadFromFile("gui/leveleditor/folder.png");
	folderUpIcon.loadFromFile("gui/leveleditor/folder_up.png");

	this->setFilePanelDirectory(this->folder, 0, true);

}

void ToolTile::setFilePanelDirectory(std::string directory, int level, bool init)
{
	auto panel = std::static_pointer_cast<tgui::Panel>(this->toolWindow->get("filePanel"));
	auto scrollBar = std::static_pointer_cast<tgui::Scrollbar>(this->toolWindow->get("fileScrollBar"));
	panel->removeAllWidgets();
	auto marker = tgui::Panel::create();
	auto renderer = marker->getRenderer();
	renderer->setBackgroundColor(sf::Color::Transparent);
	renderer->setBorderColor(sf::Color::Red);
	renderer->setBorders(tgui::Borders(1.0f));
	marker->setSize(120, 120);
	marker->setPosition(-500, 0); //Move out

	panel->add(marker, "fileMarker");

	scrollBar->setValue(0);
	cursorPos = 0;

	if (level != 0) //Directory up (You cant go out from the sprite directory!)
	{
		tgui::Picture::Ptr pic = tgui::Picture::create();
		pic->setTexture(this->folderUpIcon);
		pic->setPosition(cursorPos % 2 == 0 ? 10 : 140, 10 + cursorPos / 2 * 130);
		std::stringstream ss;
		ss << fs::path(directory).parent_path();

		auto toolTip = tgui::Label::create();
		toolTip->setText("Directory: " + ss.str());
		auto toolTipRenderer = toolTip->getRenderer();
		toolTipRenderer->setBackgroundColor(sf::Color(255, 255, 255));
		toolTipRenderer->setBorderColor(sf::Color(0, 0, 0));
		toolTipRenderer->setBorders(tgui::Borders(1.0f));
		toolTipRenderer->setPadding(5, 5, 5, 5);
		pic->setToolTip(toolTip);

		pic->connect("MouseLeft", [this]() {
			this->levelEditor->cursor->setPointer(Cursor::Pointer::Normal);
		});

		pic->connect("MouseEntered", [this]() {
			this->levelEditor->cursor->setPointer(Cursor::Pointer::Point);
		});

		pic->connect("Clicked", [this](std::string folder, int lv) {
			this->setFilePanelDirectory(folder, lv);
		}, ss.str(), level - 1);
		panel->add(pic);
		cursorPos++;
	}
	
	for (auto p : fs::directory_iterator(directory)) //Directories
	{
		if (fs::is_directory(p))
		{
			std::stringstream ss;
			ss << p;
			tgui::Picture::Ptr pic = tgui::Picture::create();

			auto toolTip = tgui::Label::create();
			toolTip->setText("Directory: "+ss.str());
			auto toolTipRenderer = toolTip->getRenderer();
			toolTipRenderer->setBackgroundColor(sf::Color(255, 255, 255));
			toolTipRenderer->setBorderColor(sf::Color(0, 0, 0));
			toolTipRenderer->setBorders(tgui::Borders(1.0f));
			toolTipRenderer->setPadding(5, 5, 5, 5);
			pic->setToolTip(toolTip);

			pic->connect("MouseLeft", [this]() {
				this->levelEditor->cursor->setPointer(Cursor::Pointer::Normal);
			});

			pic->connect("MouseEntered", [this]() {
				this->levelEditor->cursor->setPointer(Cursor::Pointer::Point);
			});

			pic->setTexture(this->folderIcon);
			pic->setPosition(cursorPos % 2 == 0 ? 10 : 140, 10 + cursorPos / 2 * 130);
			pic->connect("Clicked", [this](std::string folder, int lv) {
				this->setFilePanelDirectory(folder, lv);
			}, ss.str(), level + 1);
			panel->add(pic);
			cursorPos++;
		}
	}
	this->directory = directory;
	std::thread thread ([this]() {

		const int first = cursorPos;

		auto panel = std::static_pointer_cast<tgui::Panel>(this->toolWindow->get("filePanel"));
		auto marker = std::static_pointer_cast<tgui::Panel>(panel->get("fileMarker"));
		auto scrollBar = std::static_pointer_cast<tgui::Scrollbar>(this->toolWindow->get("fileScrollBar"));

		for (auto p : fs::directory_iterator(this->directory)) //Files
		{
			if (!fs::is_directory(p))
			{
				std::stringstream ss;
				ss << p;
				tgui::Picture::Ptr pic = tgui::Picture::create();
				if (this->thumbnails.find(ss.str()) == this->thumbnails.end()) //Make thumbnail if we dont have one yet
					this->thumbnails.insert(std::make_pair(ss.str(), this->makeThumbnail(ss.str(), sf::Vector2f(120, 120))));
				pic->setTexture(this->thumbnails.at(ss.str()).texture);
				pic->setPosition(cursorPos % 2 == 0 ? 10 : 140, 10 + cursorPos / 2 * 130);


				auto toolTip = tgui::Label::create();
				toolTip->setText("Name: " + ss.str() + "\nResoulution: " + std::to_string(this->thumbnails.at(ss.str()).originalResolution.x) + "x" + std::to_string(this->thumbnails.at(ss.str()).originalResolution.y) + "\nSize: " + this->getFileSize(static_cast<unsigned int>(fs::file_size(p))));
				auto toolTipRenderer = toolTip->getRenderer();
				toolTipRenderer->setBackgroundColor(sf::Color(255, 255, 255));
				toolTipRenderer->setBorderColor(sf::Color(0, 0, 0));
				toolTipRenderer->setBorders(tgui::Borders(1.0f));
				toolTipRenderer->setPadding(5, 5, 5, 5);
				pic->setToolTip(toolTip);

				pic->connect("Clicked", [this](tgui::Picture::Ptr pic, tgui::Panel::Ptr marker, std::string filename) {
					marker->setPosition(pic->getPosition());
					this->setSelectedSprite(filename);
				}, pic, marker, ss.str());

				pic->connect("MouseLeft", [this]() {
					this->levelEditor->cursor->setPointer(Cursor::Pointer::Normal);
				});

				pic->connect("MouseEntered", [this]() {
					this->levelEditor->cursor->setPointer(Cursor::Pointer::Point);
				});
				panel->add(pic);
				if (cursorPos == first)
				{
					marker->setPosition(pic->getPosition());
					this->setSelectedSprite(ss.str());
				}

				if (ss.str() == this->selectedFilename)
				{
					marker->setPosition(pic->getPosition());
				}
				cursorPos++;
				scrollBar->setMaximum(static_cast<unsigned int>(std::max(std::ceil(static_cast<float>(cursorPos) / 2.0f)*130.0f + 10.0f, panel->getSize().y + 1)));
			}
		}
	});
	thread.detach();
}

void ToolTile::setSelectedSprite(std::string filename)
{
	this->selectedFilename = filename;
	this->ghostTexture = this->levelEditor->resourceManager.add(filename);
	this->ghostSprite = std::make_shared<sf::Sprite>(*this->ghostTexture);
	this->ghostSprite->setColor(sf::Color(255, 0, 0, 127));
	this->ghostSprite->setOrigin(sf::Vector2f(50.0f,this->ghostTexture->getSize().y-25.0f));
}


std::string ToolTile::getFileSize(unsigned int size)
{
	unsigned int unit = 0;
	float s = static_cast<float>(size);
	while (s > 1024.f && unit < 5)
	{
		unit++;
		s /= 1024.f;
	}
	const std::string units[6] = {
		"bytes","kb","Mb","Gb","Tb","Pb"
	};

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << s;
	return ss.str() + " " + units[unit];
}

void ToolTile::draw(sf::RenderTarget& target)
{
	if (this->placeHold)
	{
		//target.draw(*this->ghostSprite);
	}
}
void ToolTile::event(sf::Event& e)
{
	switch (e.type)
	{
	case sf::Event::MouseMoved:
	{
		if (this->placeHold)
		{
			sf::Vector2f pos = this->levelEditor->screenToWorldPosition(sf::Vector2f(static_cast<float>(e.mouseMove.x), static_cast<float>(e.mouseMove.y)));
			Tile t;
			t.sprite.setTexture(*this->ghostTexture);

			t.sprite.setOrigin(this->levelEditor->level.tileWidth,this->ghostTexture->getSize().y-(this->levelEditor->level.tileHeight));
			const sf::Vector2f tempPos = this->levelEditor->level.screenToWorld(pos);
			sf::Vector2i tileCoords = sf::Vector2i((int)std::floor(tempPos.x), (int)std::floor(tempPos.y));
			this->levelEditor->level.addTile(tileCoords,t);
			this->ghostSprite->setPosition(this->levelEditor->level.worldToScreen(sf::Vector2f(tileCoords)));
			std::cout << tileCoords.x << " " << tileCoords.y << "\n";
		}
	} break;
	case sf::Event::KeyPressed:
	{
		switch (e.key.code)
		{
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
		{
			this->snapToGrid = true;
		} break;
		}
	} break;
	case sf::Event::KeyReleased:
	{
		switch (e.key.code)
		{
		case sf::Keyboard::LShift:
		case sf::Keyboard::RShift:
		{
			this->snapToGrid = false;
		} break;
		}
	} break;
	case sf::Event::MouseButtonPressed:
	{
		if (e.mouseButton.button == sf::Mouse::Right)
		{
			this->levelEditor->cursor->setPointer(Cursor::Move);
			this->placeHold = true; //Turns on the ghost
			this->ghostSprite->setPosition(this->levelEditor->screenToWorldPosition(sf::Vector2f(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y))));
			//this->ghostSprite->setOrigin(Math::vec_mul(sf::Vector2f(this->ghostTexture->getSize()), this->originNorm));
			sf::Vector2f pos = this->levelEditor->screenToWorldPosition(sf::Vector2f(static_cast<float>(e.mouseMove.x), static_cast<float>(e.mouseMove.y)));
			Tile t;
			t.sprite.setTexture(*this->ghostTexture);

			t.sprite.setOrigin(this->levelEditor->level.tileWidth, this->ghostTexture->getSize().y - (this->levelEditor->level.tileHeight));
			const sf::Vector2f tempPos = this->levelEditor->level.screenToWorld(pos);
			sf::Vector2i tileCoords = sf::Vector2i((int)std::floor(tempPos.x), (int)std::floor(tempPos.y));
			this->levelEditor->level.addTile(tileCoords, t);
		}
	} break;
	case sf::Event::MouseButtonReleased:
	{
		if (e.mouseButton.button == sf::Mouse::Right)
		{
			this->levelEditor->cursor->setPointer(Cursor::Normal);
			//this->spawnSprite(sf::Vector2f(e.mouseButton.x, e.mouseButton.y));
			this->placeHold = false;
		}
	} break;
	case sf::Event::MouseWheelScrolled:
	{
		if (this->toolWindow->isVisible())
		{
			const auto panel = std::static_pointer_cast<tgui::Panel>(this->toolWindow->get("filePanel"));
			const sf::Vector2f shift= -this->toolWindow->getAbsolutePosition()- this->toolWindow->getChildWidgetsOffset();
			if (panel->mouseOnWidget(e.mouseWheelScroll.x + shift.x, e.mouseWheelScroll.y + shift.y))
			{
				const auto scrollBar = std::static_pointer_cast<tgui::Scrollbar>(this->toolWindow->get("fileScrollBar"));
				scrollBar->setValue(Math::limitint(scrollBar->getValue() - static_cast<int>(e.mouseWheelScroll.delta)*scrollBar->getArrowScrollAmount(), 0, scrollBar->getMaximum()));
			}
		}
	} break;
	}
}

ToolTile::Thumbnail ToolTile::makeThumbnail(std::string filename, sf::Vector2f targetSize)
{
	sf::Texture tempTexture;
	tempTexture.loadFromFile(filename);
	tempTexture.setSmooth(true);
	sf::RenderTexture target;
	const sf::Vector2f size = sf::Vector2f(tempTexture.getSize());
	target.create(static_cast<unsigned int>(targetSize.x), static_cast<unsigned int>(targetSize.y), false);
	target.clear(sf::Color::Transparent);
	sf::Sprite sprite(tempTexture);
	if (size.x == size.y) sprite.setScale(targetSize.x / size.x, targetSize.y / size.y);
	else if (size.x < size.y)
	{
		const float ratio = targetSize.y / size.y;
		sprite.setScale(ratio, ratio);
		sprite.setPosition((targetSize.x - ratio*tempTexture.getSize().x) / 2.0f, 0.0f);
	}
	else if (size.x > size.y)
	{
		const float ratio = targetSize.x / size.x;
		sprite.setScale(ratio, ratio);
		sprite.setPosition(0.0f, (targetSize.y - ratio*tempTexture.getSize().y) / 2.0f);
	}
	target.draw(sprite);
	target.display();

	return Thumbnail(target.getTexture(), tempTexture.getSize());
}

bool ToolTile::isFile(std::string filename)
{
	std::fstream fsr(filename);
	return fsr.good();
}

void ToolTile::selected()
{
	this->toolWindow->show();
}
void ToolTile::unselected()
{
	this->toolWindow->hide();
}
