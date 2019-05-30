#include "../LevelEditor/ColorPicker.hpp"

#include "../LevelEditor/LevelEditor.hpp"

ColorPickerWindow::ColorPickerWindow(tgui::Gui& gui)
{
    this->shader.loadFromMemory(
		"const vec2 size=vec2(300,300);\n"
		"const float PI=3.14159265;\n"
		"uniform float angle;\n"
		"const float halfSide = 0.282842; //sin45*0.4\n"
		"vec3 hsv2rgb(vec3 c)\n"
		"{\n"
		"    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
		"    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
		"    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
		"}\n"
		"void main()\n"
		"{\n"
		"	vec2 position = gl_FragCoord.xy/size;\n"
		"	if(position.x> 0.5-halfSide&&position.x< 0.5+halfSide&&position.y>0.5-halfSide&&position.y<0.5+halfSide)\n"
		"	{\n"
		"		float a=(position.x-(0.5-halfSide))/(2.0*halfSide);\n"
		"		float v=(position.y-(0.5-halfSide))/(2.0*halfSide);\n"
		"		gl_FragColor = vec4(hsv2rgb(vec3(angle,a,v)),1);\n"
		"		return;\n"
		"	}\n"
		"	float dist = distance(vec2(0.5,0.5),position);\n"
		"	//Empty space\n"
		"	if(dist < 0.4 || dist > 0.5)\n"
		"	{\n"
		"		gl_FragColor = vec4(0,0,0,0);\n"
		"		return;\n"
		"	}\n"
		"	//Circle\n"
		"	float angle2 = atan(position.x-0.5,position.y-0.5)+(PI/2.0);\n"
		"	if(angle2<0.0) angle2 += 2.0*PI;\n"
		"    gl_FragColor =vec4(hsv2rgb(vec3(angle2/(2.0*PI),1,1)),1);\n"
		"}",sf::Shader::Fragment);
    this->window = tgui::ChildWindow::create();
    this->window->setTitle("HSV Color Picker");
    this->window->setSize(310,460);
    this->window->connect("Closed",[&](){
		this->window->hide();
	});
	this->window->keepInParent();
    this->window->hide();

    tgui::Canvas::Ptr canvas = tgui::Canvas::create({300.f,300.f});
    canvas->connect("MousePressed",[&](sf::Vector2f mouse)
    {
        const sf::Vector2f center = sf::Vector2f(150.0f,150.0f);
        const float distance = Math::distance(center,mouse);
        if(distance<150&&distance>120)
        {
            this->holdOutside = true;
            float angle =std::atan2(mouse.y-150.0f,mouse.x-150.0f)+Math::PI;
            this->hsv.x = Math::rad_to_deg(angle);
            this->rgb = Math::hsv_to_rgb(this->hsv);
            this->update();
        }
		else if(mouse.x>this->rectMin&&mouse.x<this->rectMax&&mouse.y>this->rectMin&&mouse.y<this->rectMax)
        {
            this->holdInside = true;
			const sf::Vector2f finalPos=sf::Vector2f(Math::limit(mouse.x,this->rectMin,this->rectMax),Math::limit(mouse.y,this->rectMin,this->rectMax));
			hsv.y = (finalPos.x - this->rectMin)/this->rectSide;
			hsv.z = 1.0f-(finalPos.y - this->rectMin)/this->rectSide;
			this->rgb = Math::hsv_to_rgb(this->hsv);
			this->update();
        }
    });


    canvas->setPosition(5,5);
    canvas->clear(sf::Color(230,230,230)); //Default background color of tgui
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(300,300));
    sf::RenderStates states;
    states.shader = &this->shader;
    canvas->draw(rect,states);
    canvas->display();

    this->window->add(canvas,"colorPickerCanvas");

    auto rLabel = tgui::Label::create();
    rLabel->setText("R:");
    rLabel->setPosition(5,310);
    rLabel->setSize(20,20);
    this->window->add(rLabel);
    auto rValue = tgui::EditBox::create();
    rValue->setSize(120,20);
    rValue->setPosition(30,310);
    rValue->setInputValidator("^(1?[0-9]{1,2}|2[0-4][0-9]|25[0-5]|)$");
    rValue->connect("TextChanged",[&](sf::String text){
		if(std::string(text).empty()) return;
		this->rgb.x = static_cast<float>(std::atof(std::string(text).c_str()))/255.0f;
		this->hsv = Math::rgb_to_hsv(this->rgb);
		this->update();
	});
    this->window->add(rValue,"rValue");

    auto gLabel = tgui::Label::create();
    gLabel->setText("G:");
    gLabel->setPosition(5,340);
    gLabel->setSize(20,20);
    this->window->add(gLabel);
    auto gValue = tgui::EditBox::create();
    gValue->setSize(120,20);
    gValue->setPosition(30,340);
    gValue->setInputValidator("^(1?[0-9]{1,2}|2[0-4][0-9]|25[0-5]|)$");
	gValue->connect("TextChanged",[&](sf::String text){
		if(std::string(text).empty()) return;
		this->rgb.y = static_cast<float>(std::atof(std::string(text).c_str()))/255.0f;
		this->hsv = Math::rgb_to_hsv(this->rgb);
		this->update();
	});
    this->window->add(gValue,"gValue");

    auto bLabel = tgui::Label::create();
    bLabel->setText("B:");
    bLabel->setPosition(5,370);
    bLabel->setSize(20,20);
    this->window->add(bLabel);
    auto bValue = tgui::EditBox::create();
    bValue->setSize(120,20);
    bValue->setPosition(30,370);
    bValue->setInputValidator("^^(1?[0-9]{1,2}|2[0-4][0-9]|25[0-5]|)$");
    bValue->connect("TextChanged",[&](sf::String text){
		if(std::string(text).empty()) return;
		this->rgb.z = static_cast<float>(std::atof(std::string(text).c_str()))/255.0f;
		this->hsv = Math::rgb_to_hsv(this->rgb);
		this->update();
	});
    this->window->add(bValue,"bValue");

    auto hLabel = tgui::Label::create();
    hLabel->setText("H:");
    hLabel->setPosition(160,310);
    hLabel->setSize(20,20);
    this->window->add(hLabel);
    auto hValue = tgui::EditBox::create();
    hValue->setSize(120,20);
    hValue->setPosition(185,310);
    hValue->setInputValidator("^-?[0-9]{0,5}$");
    hValue->setMaximumCharacters(5);
    this->window->add(hValue,"hValue");
    hValue->connect("TextChanged",[&](sf::String text){
		if(std::string(text).empty()) return;
		this->hsv.x = static_cast<float>(std::atof(std::string(text).c_str()))/255.0f;
		this->rgb = Math::hsv_to_rgb(this->hsv);
		this->update();
	});

    auto sLabel = tgui::Label::create();
    sLabel->setText("S:");
    sLabel->setPosition(160,340);
    sLabel->setSize(20,20);
    this->window->add(sLabel);
    auto sValue = tgui::EditBox::create();
    sValue->setSize(120,20);
    sValue->setPosition(185,340);
    sValue->setInputValidator("^[0]*([0-9]?[0-9]?|100)$");
    sValue->connect("TextChanged",[&](sf::String text){
		if(std::string(text).empty()) return;
		this->hsv.y = static_cast<float>(std::atof(std::string(text).c_str()))/255.0f;
		this->rgb = Math::hsv_to_rgb(this->hsv);
		this->update();
	});
    this->window->add(sValue,"sValue");

    auto vLabel = tgui::Label::create();
    vLabel->setText("V:");
    vLabel->setPosition(160,370);
    vLabel->setSize(20,20);
    this->window->add(vLabel);
    auto vValue = tgui::EditBox::create();
    vValue->setSize(120,20);
    vValue->setPosition(185,370);
    vValue->setInputValidator("^[0]*([0-9]?[0-9]?|100)$");
    vValue->connect("TextChanged",[&](sf::String text){
		if(std::string(text).empty()) return;
		this->hsv.z = static_cast<float>(std::atof(std::string(text).c_str()))/255.0f;
		this->rgb = Math::hsv_to_rgb(this->hsv);
		this->update();
	});
    this->window->add(vValue,"vValue");

    auto aLabel = tgui::Label::create();
    aLabel->setText("A:");
    aLabel->setPosition(5,400);
    aLabel->setSize(20,20);
    this->window->add(aLabel);
    auto aValue = tgui::Slider::create();
    aValue->setSize(275,8);
    aValue->setPosition(30,406);
    aValue->setMinimum(0);
    aValue->setMaximum(255);
    aValue->setValue(static_cast<int>(std::floor(this->alpha*255.0f)));
    aValue->connect("ValueChanged ",[&](int value){
		this->alpha = static_cast<float>(value)/255.0f;
	});
    this->window->add(aValue,"aValue");

    auto applyButton = tgui::Button::create();
    applyButton->setPosition(5,430);
    applyButton->setSize(70,25);
	applyButton->setText("Apply");
	applyButton->connect("Clicked",[&](){
		this->colorPicker->setColor(sf::Color(static_cast<int>(std::floor(this->rgb.x*255.0f)), static_cast<int>(std::floor(this->rgb.y*255.0f)),
			static_cast<int>(std::floor(this->rgb.z*255.0f)), static_cast<int>(std::floor(this->alpha*255.0f))));
	});
	this->window->add(applyButton);

    auto okButton = tgui::Button::create();
    okButton->setPosition(90,430);
    okButton->setSize(70,25);
	okButton->setText("Ok");
	okButton->connect("Clicked",[&](){
		this->colorPicker->setColor(sf::Color(static_cast<int>(std::floor(this->rgb.x*255.0f)), static_cast<int>(std::floor(this->rgb.y*255.0f)),
			static_cast<int>(std::floor(this->rgb.z*255.0f)), static_cast<int>(std::floor(this->alpha*255.0f))));
		this->window->hide();
	});
	this->window->add(okButton);

    gui.add(this->window);

    this->indicatorHue.setOutlineColor(sf::Color::Black);
    this->indicatorHue.setOutlineThickness(2.f);
    this->indicatorHue.setFillColor(sf::Color::Transparent);
    this->indicatorHue.setOrigin(15.0f,3.5f);
    this->indicatorHue.setSize(sf::Vector2f(30.f,7.f));

    this->indicatorSV.setRadius(3);
    this->indicatorSV.setOrigin(3,3);
    this->indicatorSV.setFillColor(sf::Color::Transparent);
    this->indicatorSV.setOutlineColor(sf::Color::Black);
    this->indicatorSV.setOutlineThickness(2);

    this->update();
}

void ColorPickerWindow::update()
{
	//Alpha
	std::static_pointer_cast<tgui::Slider>(this->window->get("aValue"))->setValue(static_cast<int>(std::floor(this->alpha*255.0f)));
	//Colorpicker canvas
    tgui::Canvas::Ptr canvas = std::static_pointer_cast<tgui::Canvas>(this->window->get("colorPickerCanvas"));
    canvas->clear(sf::Color(230,230,230)); //Default background color of tgui

    sf::RectangleShape picker;
    picker.setSize(sf::Vector2f(300,300));

    const float angle = Math::deg_to_rad(this->hsv.x);
    this->shader.setUniform("angle",angle/Math::TWO_PI); //Shader's converter expects angle as a normal
    canvas->draw(picker,&this->shader);

    //Indicator for Hue
	const float visibleAngle= angle+Math::PI;
    this->indicatorHue.setPosition(150.f+std::cos(visibleAngle)*135.f,150.f+std::sin(visibleAngle)*135.f);
    this->indicatorHue.setRotation(Math::rad_to_deg(visibleAngle));
    canvas->draw(this->indicatorHue);

    //Indicator for S/V
    this->indicatorSV.setPosition(this->rectMin+(this->hsv.y*this->rectSide),this->rectMin+((1.0f-this->hsv.z)*this->rectSide));
    canvas->draw(this->indicatorSV);

    //Display the canvas
    canvas->display();

    std::static_pointer_cast<tgui::EditBox>(this->window->get("rValue"))->setText(std::to_string(static_cast<int>(this->rgb.x*255.f)));
    std::static_pointer_cast<tgui::EditBox>(this->window->get("gValue"))->setText(std::to_string(static_cast<int>(this->rgb.y*255.f)));
    std::static_pointer_cast<tgui::EditBox>(this->window->get("bValue"))->setText(std::to_string(static_cast<int>(this->rgb.z*255.f)));
    std::static_pointer_cast<tgui::EditBox>(this->window->get("hValue"))->setText(std::to_string(static_cast<int>(this->hsv.x)));//
    std::static_pointer_cast<tgui::EditBox>(this->window->get("sValue"))->setText(std::to_string(static_cast<int>(this->hsv.y*100.f)));
    std::static_pointer_cast<tgui::EditBox>(this->window->get("vValue"))->setText(std::to_string(static_cast<int>(this->hsv.z*100.f)));
}

void ColorPickerWindow::event(sf::Event& e)
{
    if(e.type == sf::Event::MouseButtonReleased)
    {
        this->holdOutside = false;
        this->holdInside = false;
    }
    else if(e.type == sf::Event::MouseMoved)
    {
        if(this->holdOutside)
        {
            const sf::Vector2f center = this->window->get("colorPickerCanvas")->getAbsolutePosition()+sf::Vector2f(150.0f,150.0f);
            float angle =std::atan2(e.mouseMove.y-center.y,e.mouseMove.x-center.x)+Math::PI;
            this->hsv.x = Math::rad_to_deg(angle);
            this->rgb = Math::hsv_to_rgb(this->hsv);
            this->update();
        }
        else if(this->holdInside)
        {
			const sf::Vector2f mouse = sf::Vector2f(static_cast<float>(e.mouseMove.x),static_cast<float>(e.mouseMove.y))-this->window->get("colorPickerCanvas")->getAbsolutePosition();
			const sf::Vector2f finalPos=sf::Vector2f(Math::limit(static_cast<float>(mouse.x),this->rectMin,this->rectMax),
													 Math::limit(static_cast<float>(mouse.y), this->rectMin, this->rectMax));
			hsv.y = (finalPos.x - this->rectMin)/this->rectSide;
			hsv.z = 1.0f-(finalPos.y - this->rectMin)/this->rectSide;
			this->rgb = Math::hsv_to_rgb(this->hsv);
			this->update();
        }
    }
}

void ColorPickerWindow::show(ColorPicker* colorPicker)
{
    this->colorPicker = colorPicker;
    this->rgb = sf::Vector3f(colorPicker->getColor().r/255.0f,colorPicker->getColor().g/255.0f,colorPicker->getColor().b/255.0f);
    this->hsv = Math::rgb_to_hsv(this->rgb);
    this->alpha = colorPicker->getColor().a/255.0f;
	this->update();
    this->window->setPosition(colorPicker->getWidget()->getAbsolutePosition()+sf::Vector2f(colorPicker->getWidget()->getSize().x+10.f,0));
	this->window->show();
	this->window->moveToFront();
	this->window->focus();
}

ColorPicker::ColorPicker()
{
	create();
}

ColorPicker::ColorPicker(sf::Color color)
{
	this->color = color;
	create();
}

void ColorPicker::create()
{
	this->panel = tgui::Panel::create();
	this->panel->setSize(25,25);
	this->panel->getRenderer()->setBorderColor(sf::Color::Black);
	this->panel->setBackgroundColor(this->color);
	this->panel->connect("Clicked",[&](){
		Tool::levelEditor->colorPickerWindow->show(this);
	});
}

sf::Color ColorPicker::getColor()
{
	return this->color;
}

void ColorPicker::setColor(sf::Color color)
{
	this->color = color;
	this->panel->setBackgroundColor(color);
}

tgui::Panel::Ptr ColorPicker::getWidget()
{
	return this->panel;
}

