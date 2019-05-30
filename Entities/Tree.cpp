#include "Tree.hpp"

sf::Shader* client::Tree::shader = nullptr;
int client::Tree::shaderUsers = 0;

client::Tree::Tree(sf::Vector2f position, ResourceManager & resMg, Physics & phy, sf::Vector2f resolution) : texture(resMg.addTexture("sprites/trees/tree2.png"))
{
	const sf::Vector2f pos = Math::worldToScreen(position);

	const sf::Vector2f s = sf::Vector2f(texture.getSize());
	AABB = sf::FloatRect(pos + sf::Vector2f(-s.x / 2.0f, -s.y), s);
	va.setPrimitiveType(sf::Points);
	va.append(pos);
	this->screen_y = pos.y;
	body = std::make_shared<CircleBody>(bodyRadius);
	body->setPosition(position*100.0f);
	phy.addBody(body);

	size = Math::vec_div(s, resolution)*2.0f;

	sf::Texture& st = resMg.addTexture("sprites/shadow/circle.png");
	shadow.setTexture(st);
	shadow.setOrigin(sf::Vector2f(st.getSize()) / 2.0f);
	shadow.setPosition(pos);
	shadow.setScale(4.0f, 4.0f);
	shadow.setColor(sf::Color(255, 255, 255, 100));

	if (!shader)
	{
		shader = new sf::Shader();
		shader->loadFromFile("shaders/grass.vert", "shaders/grass.geom", "shaders/grass.frag");
	}
	shaderUsers++;
}

void client::Tree::drawPre(sf::RenderTarget & target) const
{
	target.draw(shadow);
}

void client::Tree::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.shader = shader;
	shader->setUniform("strength", strength);
	shader->setUniform("size", size);
	states.texture = &texture;
	target.draw(va, states);
}

void client::Tree::update(const sf::Time & time, const sf::Time & dt, Wind & wind)
{
	strength = wind.getValue() * 0.05f;
	strength.y = 1.0f - strength.y;
}
