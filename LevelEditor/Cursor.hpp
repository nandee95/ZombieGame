#include <SFML/Graphics.hpp>

class Cursor : public sf::Drawable
{
public:
    enum Pointer
    {
        Normal,
        Question,
        Point,
        Grab,
        Block,
        Text,
        Move,
        HorizontalArrow,
        VerticalArrow,
        DiagonalArrow1,
        DiagonalArrow2,
        ___Size
    };

    const int parser[Pointer::___Size][6]=  //x y width height originx originy
    {
        {0,0,64,64,0,0},
        {64,0,64,64,0,0},
        {128,0,64,64,32,0},
        {192,0,64,64,32,32},
        {256,0,64,64,32,32},
        {320,0,64,64,32,32},
        {0,64,64,64,32,32},
        {64,64,64,64,32,32},
        {128,64,64,64,32,32},
        {192,64,64,64,32,32},
        {256,64,64,64,32,32}
    };
private:
    Pointer pointer=Normal;
    sf::Sprite sprites[Pointer::___Size];
    sf::Texture textures[Pointer::___Size];
    sf::Image image;
public:
    Cursor(std::string filename,float scale);
    void update(sf::Vector2f position);
    void setPointer(Pointer state);
    virtual void draw(sf::RenderTarget& target,sf::RenderStates states) const;
};
