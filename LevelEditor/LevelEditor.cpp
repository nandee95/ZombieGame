#include "../LevelEditor/LevelEditor.hpp"

#include "../LevelEditor/ColorPicker.hpp"

LevelEditor::LevelEditor()
{
    //Create globals
    Tool::levelEditor = this;
	this->resetView();
	std::shared_ptr<Window> window = Globals::window.lock();
    this->swConvert = sf::Vector2f(this->view.getSize().x/static_cast<float>(window->getSize().x),this->view.getSize().y/static_cast<float>(window->getSize().y));
    this->cursor = std::make_unique<Cursor>("gui/leveleditor/cursor.png",0.68f);
    window->setMouseCursorVisible(false);
    this->oldPosition=sf::Vector2f(sf::Mouse::getPosition(*window));
    init_gui();
}

LevelEditor::~LevelEditor()
{
    colorPickerWindow.reset();
}

void LevelEditor::resetView()
{
	std::shared_ptr<Window> window = Globals::window.lock();
	const float ratio = static_cast<float>(window->getSize().x) / static_cast<float>(window->getSize().y);
	this->view.setSize(1000.f*ratio, 1000.f);
	this->view.setCenter(0.f, 0.f);
}

void LevelEditor::event(sf::Event& e)
{
    this->colorPickerWindow->event(e);
    if(this->gui.handleEvent(e)&&e.type!=sf::Event::MouseMoved) return;

    switch(e.type)
    {
    case sf::Event::MouseMoved:
    {
        const sf::Vector2f newPosition(static_cast<float>(e.mouseMove.x),static_cast<float>(e.mouseMove.y));
        this->cursor->update(newPosition);
        if(isGrabbed)
        {
            const sf::Vector2f diff = oldPosition-newPosition;
            this->view.setCenter(this->view.getCenter()+this->screenToWorldScale(diff));
        }

        oldPosition = newPosition;
    }
    break;
    case sf::Event::MouseWheelMoved:
    {
        this->setZoom(Math::limitint(static_cast<int>(zoom+e.mouseWheel.delta),0,6));
    }
    break;
    case sf::Event::MouseButtonPressed:
    {
        switch(e.mouseButton.button)
        {
        case sf::Mouse::Middle: //Grab
        {
            this->cursor->setPointer(Cursor::Pointer::Grab);
            this->isGrabbed = true;
        }
        break;
        }
    }
    break;
    case sf::Event::MouseButtonReleased:
    {
        switch(e.mouseButton.button)
        {
        case sf::Mouse::Middle: //Grab
        {
            this->cursor->setPointer(Cursor::Pointer::Normal);
            this->isGrabbed = false;
        }
        break;
        }
    }
    break;
    }
    this->tools[this->currentTool]->event(e);
}

void LevelEditor::update()
{

}

void LevelEditor::draw(sf::RenderTarget& target)
{
	target.clear(sf::Color(30,255,255));
    target.setView(this->view);
    if(this->showAxes)
    {
        sf::VertexArray axes(sf::Lines);
        axes.append(sf::Vertex(sf::Vector2f(0.0f, 44.7214f),sf::Color::Red));
        axes.append(sf::Vertex(sf::Vector2f(0.0f,0.0f),sf::Color::Red));
        axes.append(sf::Vertex(sf::Vector2f(0.0f,0.0f),sf::Color::Blue));
        axes.append(sf::Vertex(sf::Vector2f(89.4427f,0.0f),sf::Color::Blue));
        target.draw(axes);
    }
	level.draw(target,this->view);
	/*
	const float sideLen = 100.0f;
	const float xAmt = 50.0f;
	const float yAmt = 25.0f;

	sf::Vector2f coord;

	sf::Vector2f mouse = this->screenToWorldPosition(sf::Vector2f(sf::Mouse::getPosition(*Globals::window.lock())));
	mouse.y += yAmt ;
	coord.x = ((mouse.y / yAmt) + (mouse.x / xAmt)) / 2.0f;
	coord.y = (mouse.y / yAmt) - coord.x;

	this->zoomLabel->setText("."+std::to_string((int)std::floor(coord.x))+ " " +std::to_string((int)std::floor(coord.y))+".");

	for (int a = 0; a<40; a++) {
		for (int b = -8; b<10; b++) {
			if ((b & 1) != (a & 1)) continue;
			int x = (a + b) / 2;
			int y = (a - b) / 2;
			sf::RectangleShape cs;
			cs.setSize(sf::Vector2f(10, 10));
			cs.setOrigin(5, 5);
			cs.setFillColor(sf::Color::Red);
			sf::Vector2f pos = sf::Vector2f((y-x) * xAmt,(y+x) * yAmt);

			cs.setPosition(pos);

			target.draw(cs);
			if (std::floor(coord.x) == x && std::floor(coord.y) == y)
			{
				sf::VertexArray va(sf::Quads);

				const sf::Vector2f shiftVec = sf::Vector2f(xAmt*x, yAmt*x) + sf::Vector2f(-xAmt*y, yAmt*y);
				va.append(sf::Vertex(shiftVec + sf::Vector2f(-xAmt, 0), sf::Color::Red));
				va.append(sf::Vertex(shiftVec + sf::Vector2f(0, yAmt), sf::Color::Red));
				va.append(sf::Vertex(shiftVec + sf::Vector2f(0, yAmt), sf::Color::Red));
				va.append(sf::Vertex(shiftVec + sf::Vector2f(xAmt, 0), sf::Color::Red));
				va.append(sf::Vertex(shiftVec + sf::Vector2f(xAmt, 0), sf::Color::Red));
				va.append(sf::Vertex(shiftVec + sf::Vector2f(0, -yAmt), sf::Color::Red));
				va.append(sf::Vertex(shiftVec + sf::Vector2f(0, -yAmt), sf::Color::Red));
				va.append(sf::Vertex(shiftVec + sf::Vector2f(-xAmt, 0), sf::Color::Red));
				target.draw(va);
			}
			sf::VertexArray va(sf::Lines);

			const sf::Vector2f shiftVec = sf::Vector2f(xAmt*x, yAmt*x) + sf::Vector2f(-xAmt*y, yAmt*y);
			va.append(sf::Vertex(shiftVec + sf::Vector2f(-xAmt, 0), sf::Color::White));
			va.append(sf::Vertex(shiftVec + sf::Vector2f(0, yAmt), sf::Color::White));
			va.append(sf::Vertex(shiftVec + sf::Vector2f(0, yAmt), sf::Color::White));
			va.append(sf::Vertex(shiftVec + sf::Vector2f(xAmt, 0), sf::Color::White));
			va.append(sf::Vertex(shiftVec + sf::Vector2f(xAmt, 0), sf::Color::White));
			va.append(sf::Vertex(shiftVec + sf::Vector2f(0, -yAmt), sf::Color::White));
			va.append(sf::Vertex(shiftVec + sf::Vector2f(0, -yAmt), sf::Color::White));
			va.append(sf::Vertex(shiftVec + sf::Vector2f(-xAmt, 0), sf::Color::White));
			target.draw(va);
		}
	}
	*/
    this->tools[this->currentTool]->draw(target);
    target.setView(target.getDefaultView());
    this->gui.draw();
    target.draw(*this->cursor);
}

void LevelEditor::init_gui()
{
    try
    {
        std::shared_ptr<Window> window = Globals::window.lock();
        this->gui.setWindow(*window);

        menuBar = tgui::MenuBar::create();
        menuBar->setSize(window->getSize().x, 20);
        menuBar->addMenu("File WIP");
        menuBar->addMenuItem("Open");
        menuBar->addMenuItem("Save");
        menuBar->addMenuItem("Save as..");
        menuBar->addMenuItem("Back to the main menu");
        menuBar->addMenu("View");
		menuBar->addMenuItem("Toggle axes");
		menuBar->addMenuItem("Reset view");
        menuBar->addMenu("Tools");
        for(int i=0; i<sizeof this->tools/4; i++)
        {
            this->tools[i]->init_gui(gui);
            menuBar->addMenuItem(this->tools[i]->name);
        }
        menuBar->addMenu("Settings");
        menuBar->addMenuItem("Level settings");

        menuBar->connect("Focused Unfocused", [&]()
        {
			this->menuBar->moveToFront();
			this->zoomLabel->moveToFront();
        });
        menuBar->connect("MenuItemClicked", [&](sf::String item)
        {
            if(item=="Toggle axes")
            {
                this->showAxes = ! this->showAxes;
            }
			if (item == "Reset view")
			{
				this->resetView();
			}
            //Tool selected
            for(int i=0; i<sizeof this->tools/4; i++)
            {
                if(this->tools[i]->name == item)
                {
                    this->tools[i]->selected();
                    this->currentTool=i;
                    //Unselect others
                    for(int i=0; i<sizeof this->tools/4; i++)
                    {
                        if(this->tools[i]->name != item)
                        {
                            this->tools[i]->unselected();
                        }
                    }
                }
            }
        });

        gui.add(menuBar);

        zoomLabel = tgui::Label::create();

        zoomLabel->setText("Zoom: 100%");
        zoomLabel->setPosition(window->getSize().x* 0.5f,0);
        zoomLabel->setSize(window->getSize().x * 0.1f, 20);

		gridLabel = tgui::Label::create();
		gridLabel->setText("Coordinates: 100%");
		gridLabel->setPosition(window->getSize().x * 0.6f, 0);
		gridLabel->setSize(window->getSize().x * 0.1f, 20);

        zoomLabel->connect("Clicked", [&]()   //Reset zoom on click event
        {
            this->setZoom(4); // 100%
        });
        gui.add(zoomLabel);

        colorPickerWindow = std::make_shared<ColorPickerWindow>(gui);

    }
    catch (const tgui::Exception& e)
    {
        Error::Critical("Failed to load tgui\nMessage:"+std::string(e.what()));
    }
}

sf::Vector2f LevelEditor::screenToWorldScale(sf::Vector2f screen)
{
    return Math::vec_mul(screen,this->swConvert)/(zoomValue[zoom]/100.f);
}

sf::Vector2f LevelEditor::screenToWorldPosition(sf::Vector2f screen)
{
    return this->screenToWorldScale(screen)+(this->view.getCenter()-this->view.getSize()/2.f);
}

void LevelEditor::setZoom(int zoom)
{
    const int prevZoom = zoomValue[this->zoom];
    this->zoom = zoom;
    this->zoomLabel->setText("Zoom: "+std::to_string(this->zoomValue[this->zoom])+"%");
    this->view.zoom(static_cast<float>(prevZoom)/static_cast<float>(zoomValue[this->zoom]));
}

int LevelEditor::getZoomValue()
{
    return this->zoom;
}
