#include "../LevelEditor/ToolSprite.hpp"
#include "../LevelEditor/LevelEditor.hpp"

ToolSprite::ToolSprite()
{
    this->name = "Sprite tool";
}

void ToolSprite::init_gui(tgui::Gui& gui)
{
    this->toolWindow = tgui::ChildWindow::create();
    this->toolWindow->setSize(320,900);
    this->toolWindow->setPosition(40,60);
    this->toolWindow->setTitle(this->name);
    this->toolWindow->connect("Closed",[&]()
    {
        this->toolWindow->hide();
    });
    this->toolWindow->keepInParent();
    this->toolWindow->hide();
    gui.add(this->toolWindow);


	auto distanceLabel = tgui::Label::create();
	distanceLabel->setText("Distance: 0%");
	distanceLabel->setPosition(10, 10);
	this->toolWindow->add(distanceLabel);

	auto distanceSlider = tgui::Slider::create();
	distanceSlider->setSize(300,10);
	distanceSlider->setPosition(10, 40);
	distanceSlider->setMaximum(100);
	distanceSlider->setMinimum(0);
	distanceSlider->setValue(0);
	distanceSlider->connect("ValueChanged", [this](tgui::Label::Ptr label,int newVal) {
		label->setText("Distance: "+std::to_string(newVal)+"%");
		this->distance = static_cast<float>(newVal)/100.0f;
	}, distanceLabel);
	this->toolWindow->add(distanceSlider);

	auto colorLabel = tgui::Label::create();
	colorLabel->setText("Color:");
	colorLabel->setPosition(10, 60);
	this->toolWindow->add(colorLabel);

	auto colorPickerWidget = this->colorPicker.getWidget();
	colorPickerWidget->setPosition(10, 80);
	colorPickerWidget->setSize(50, 20);
	this->toolWindow->add(colorPickerWidget);

	auto originLabel = tgui::Label::create();
	originLabel->setText("Origin:");
	originLabel->setPosition(10,110);
	this->toolWindow->add(originLabel);
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			auto originRadioButton = tgui::RadioButton::create();
			originRadioButton->setSize(20,20);
			originRadioButton->setPosition(10+x*25,140+y*25);
			originRadioButton->connect("Checked", [this](int x,int y) {
				this->originNorm.x = static_cast<float>(x) / 2.0f;
				this->originNorm.y = static_cast<float>(y) / 2.0f;
			},x,y);
			if (x == y && x==1) originRadioButton->check();
			this->toolWindow->add(originRadioButton);
		}
	}


    auto filePanel = tgui::Panel::create();
    filePanel->setSize(270,600);
    filePanel->setPosition(10,290);

    this->toolWindow->add(filePanel,"filePanel");

	auto fileScrollBar = tgui::Scrollbar::create();
	fileScrollBar->setSize(30, filePanel->getSize().y);
	fileScrollBar->setPosition(filePanel->getPosition().x+ filePanel->getSize().x,filePanel->getPosition().y);
	fileScrollBar->setArrowScrollAmount(65);
	fileScrollBar->setLowValue(static_cast<unsigned int>(filePanel->getSize().y));
	fileScrollBar->connect("ValueChanged", [this](tgui::Panel::Ptr panel,int newValue) {
		const int dist = this->prevScrollValue - newValue;
		for (auto& widget : panel->getWidgets())
		{
			widget->setPosition(widget->getPosition().x, widget->getPosition().y + dist);
		}
		this->prevScrollValue = newValue;
	},filePanel);
	this->toolWindow->add(fileScrollBar,"fileScrollBar");

	int count = 0;

	folderIcon.loadFromFile("gui/leveleditor/folder.png");
	folderUpIcon.loadFromFile("gui/leveleditor/folder_up.png");

	this->setFilePanelDirectory(this->folder,0,true);

	this->ghostSprite->setColor(sf::Color(255,255,255,127));
}

void ToolSprite::setFilePanelDirectory(std::string directory,int level,bool init)
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
	marker->setPosition(-500,0); //Move out

	panel->add(marker, "fileMarker");

	scrollBar->setValue(0);
	int count = 0;
	
	if(level!=0) //Directory up (You cant go out from the sprite directory!)
	{
		tgui::Picture::Ptr pic = tgui::Picture::create();
		pic->setTexture(this->folderUpIcon);
		pic->setPosition(count % 2 == 0 ? 10 : 140, 10 + count / 2 * 130);
		std::stringstream ss;
		ss << fs::path(directory).parent_path();

		pic->connect("Clicked", [this](std::string folder, int lv) {
			this->setFilePanelDirectory(folder,lv);
		},ss.str(),level-1);
		panel->add(pic);
		count++;
	}

	for (auto p : fs::directory_iterator(directory)) //Directories
	{
		if (fs::is_directory(p))
		{
			std::stringstream ss;
			ss << p;
			tgui::Picture::Ptr pic = tgui::Picture::create();
			pic->setTexture(this->folderIcon);
			pic->setPosition(count % 2 == 0 ? 10 : 140, 10 + count / 2 * 130);
			pic->connect("Clicked", [this](std::string folder,int lv) {
				this->setFilePanelDirectory(folder, lv);
			},ss.str(),level+1);
			panel->add(pic);
			count++;
		}
	}
	const int first = count;
	for (auto p : fs::directory_iterator(directory)) //Files
	{
		if (!fs::is_directory(p))
		{
			std::stringstream ss;
			ss << p;
			tgui::Picture::Ptr pic = tgui::Picture::create();
			if(this->thumbnails.find(ss.str())==this->thumbnails.end()) //Make thumbnail if we dont have one yet
				this->thumbnails.insert(std::make_pair(ss.str(), this->makeThumbnail(ss.str(), sf::Vector2f(120, 120))));
			pic->setTexture(this->thumbnails.at(ss.str()).texture);
			pic->setPosition(count % 2 == 0 ? 10 : 140, 10 + count / 2 * 130);

			auto toolTip = tgui::Label::create();
			toolTip->setText("Name: "+ss.str()+"\nResoulution: "+std::to_string(this->thumbnails.at(ss.str()).originalResolution.x)+"x"+std::to_string(this->thumbnails.at(ss.str()).originalResolution.y)+"\nSize: "+this->getFileSize(fs::file_size(p)));
			auto toolTipRenderer = toolTip->getRenderer();
			toolTipRenderer->setBackgroundColor(sf::Color(255, 255, 255));
			toolTipRenderer->setBorderColor(sf::Color(0,0,0));
			toolTipRenderer->setBorders(tgui::Borders(1.0f));
			toolTipRenderer->setPadding(5,5,5,5);
			pic->setToolTip(toolTip);
			pic->connect("Clicked", [this](tgui::Picture::Ptr pic,tgui::Panel::Ptr marker,std::string filename) {
				marker->setPosition(pic->getPosition());
				this->setSelectedSprite(filename);
			},pic,marker,ss.str());

			pic->connect("MouseLeft", [this]() {
				this->levelEditor->cursor->setPointer(Cursor::Pointer::Normal);
			});
			
			pic->connect("MouseEntered", [this]() {
				this->levelEditor->cursor->setPointer(Cursor::Pointer::Point);
			});
			panel->add(pic);
			if (init && count == first)
			{
				marker->setPosition(pic->getPosition());
				this->setSelectedSprite(ss.str());
			}

			if (ss.str() == this->selectedFilename)
			{
				marker->setPosition(pic->getPosition());
			}

			count++;
		}
	}
	scrollBar->setMaximum(std::max(std::ceil(static_cast<float>(count)/2.0f)*130.0f+10.0f,panel->getSize().y+1));
}

void ToolSprite::setSelectedSprite(std::string filename)
{

	this->selectedFilename = filename;
	this->ghostTexture = this->levelEditor->resourceManager.add(filename);
	this->ghostSprite = std::make_shared<sf::Sprite>(*this->ghostTexture);
	this->ghostSprite->setColor(sf::Color(255, 255, 255, 127));
	this->ghostSprite->setOrigin(sf::Vector2f(this->ghostTexture->getSize())/2.0f);
}


std::string ToolSprite::getFileSize(unsigned int size)
{
	unsigned int unit = 0;
	float s = static_cast<float>(size);
	while (s > 1024.f)
	{
		unit++;
		s /= 1024.f;
	}
	const std::string units[5] = {
		"kb","Mb","Gb","Tb","Pb"
	};
	
	if (unit >= 5) return "???";

	std::stringstream ss;
	ss << std::fixed << std::setprecision(2) << s;	
	return ss.str()+ " " + units[unit-1];
}

void ToolSprite::draw(sf::RenderTarget& target)
{
	if (this->placeHold)
	{
		target.draw(*this->ghostSprite);
	}
}
void ToolSprite::event(sf::Event& e)
{
	switch(e.type)
	{
	case sf::Event::MouseMoved:
	{
		if (this->placeHold)
		{
			this->ghostSprite->setPosition(this->levelEditor->screenToWorldPosition(sf::Vector2f(e.mouseMove.x, e.mouseMove.y)));
		}
	} break;
	case sf::Event::KeyPressed:
	{
		switch(e.key.code)
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
		switch(e.key.code)
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
		if(e.mouseButton.button==sf::Mouse::Right)
		{
			this->levelEditor->cursor->setPointer(Cursor::Move);
			this->placeHold = true; //Turns on the ghost
			this->ghostSprite->setPosition(this->levelEditor->screenToWorldPosition(sf::Vector2f(e.mouseButton.x, e.mouseButton.y)));
			this->ghostSprite->setOrigin(Math::vec_mul(sf::Vector2f(this->ghostTexture->getSize()), this->originNorm));
		}
	} break;
	case sf::Event::MouseButtonReleased:
	{
		if(e.mouseButton.button==sf::Mouse::Right)
		{
			this->levelEditor->cursor->setPointer(Cursor::Normal);
			this->spawnSprite(sf::Vector2f(e.mouseButton.x,e.mouseButton.y));
			this->placeHold = false;
		}
	} break;
	case sf::Event::MouseWheelScrolled:
	{
		if (this->toolWindow->isVisible())
		{
			auto panel = std::static_pointer_cast<tgui::Panel>(this->toolWindow->get("filePanel"));
			if (panel->mouseOnWidget(e.mouseWheelScroll.x,e.mouseWheelScroll.y))
			{
				const auto scrollBar = std::static_pointer_cast<tgui::Scrollbar>(this->toolWindow->get("fileScrollBar"));
				scrollBar->setValue(Math::limit(scrollBar->getValue()-(e.mouseWheelScroll.delta*scrollBar->getArrowScrollAmount()),0,scrollBar->getMaximum()));
			}
		}
	} break;
	}
}

void ToolSprite::spawnSprite(sf::Vector2f screenPos)
{
    const sf::Vector2f worldPos=this->levelEditor->screenToWorldPosition(screenPos);
	const std::shared_ptr<sf::Texture> texture = this->levelEditor->resourceManager.add(this->selectedFilename);
	std::shared_ptr<sf::Sprite> sprite=std::make_unique<sf::Sprite>(*texture);
	sprite->setPosition(worldPos-sf::Vector2f(this->levelEditor->view.getCenter().x*distance,0.0f));
	sprite->setOrigin(Math::vec_mul(sf::Vector2f(texture->getSize()), this->originNorm));
	sprite->setColor(this->colorPicker.getColor());
	this->levelEditor->levelData.push_back(std::make_pair(distance,sprite));
}

ToolSprite::Thumbnail ToolSprite::makeThumbnail(std::string filename,sf::Vector2f targetSize)
{
	sf::Texture tempTexture;
	tempTexture.loadFromFile(filename);
	tempTexture.setSmooth(true);
	sf::RenderTexture target;
	const sf::Vector2f size = sf::Vector2f(tempTexture.getSize());
	target.create(targetSize.x,targetSize.y,false);
	target.clear(sf::Color::Transparent);
	sf::Sprite sprite(tempTexture);
	if (size.x == size.y) sprite.setScale(targetSize.x/size.x,targetSize.y/size.y);
	else if(size.x < size.y)
	{
		const float ratio = targetSize.y/size.y;
		sprite.setScale(ratio,ratio);
		sprite.setPosition((targetSize.x-ratio*tempTexture.getSize().x)/2.0f,0.0f);
	}
	else if(size.x > size.y)
	{
		const float ratio = targetSize.x/size.x;
		sprite.setScale(ratio,ratio);
		sprite.setPosition(0.0f, (targetSize.y - ratio*tempTexture.getSize().y) / 2.0f);
	}
	target.draw(sprite);
	target.display();

	return Thumbnail(target.getTexture(), tempTexture.getSize());
}

bool ToolSprite::isFile(std::string filename)
{
	std::fstream fs(filename);
	return fs.good();
}

void ToolSprite::selected()
{
    this->toolWindow->show();
}
void ToolSprite::unselected()
{
    this->toolWindow->hide();
}
