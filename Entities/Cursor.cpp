#include "../LevelEditor/Cursor.hpp"

Cursor::Cursor(std::string filename,float scale)
{
    this->image.loadFromFile(filename);
    for(unsigned int i=0; i<this->Pointer::___Size; i++)
    {
        this->textures[i].loadFromImage(this->image,sf::IntRect(this->parser[i][0],this->parser[i][1],this->parser[i][2],this->parser[i][3]));
        this->textures[i].setSmooth(true);
        this->sprites[i].setTexture(this->textures[i]);
        this->sprites[i].setOrigin(static_cast<float>(parser[i][4]),static_cast<float>(parser[i][5]));
        this->sprites[i].setScale(scale,scale);
    }
}

void Cursor::update(sf::Vector2f position)
{
    this->sprites[pointer].setPosition(position);
}

void Cursor::setPointer(Pointer pointer)
{
    const Pointer before = this->pointer;
    this->pointer = pointer;
    this->sprites[this->pointer].setPosition(this->sprites[before].getPosition());
}

void Cursor::draw(sf::RenderTarget& target,sf::RenderStates states) const
{
    target.draw(this->sprites[this->pointer],states);
}
