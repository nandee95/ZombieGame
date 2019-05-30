#include "../Entities/Grass.hpp"


sf::Shader* client::Grass::shader=nullptr;
int client::Grass::shaderUsers=0;

client::Grass::Grass(std::vector<sf::Vector2f> positions, sf::Vector2f resolution, ResourceManager & resMg) :
	texture(resMg.addTexture("sprites/environment/grass.png"))
{
	va.setPrimitiveType(sf::Points);
	for (auto& pos : positions)
	{
		const sf::Int8 comm = Math::random(0, 50);
		va.append(sf::Vertex(Math::worldToScreen(pos), sf::Color(comm, Math::random(150, 200), comm, 225)));
	}
	const sf::Vector2f texSize = sf::Vector2f(texture.getSize());
	//const sf::Vector2f texSize2 = sf::Vector2f(texture.getSize()) *1.25f; // Increased AABB with 25% for deformation
																		  //AABB = sf::FloatRect(finalPos + sf::Vector2f(-texSize2.x/2.0f, -texSize2.y), texSize2);

																		  //this->screen_y = finalPos.y;
	size = Math::vec_div(texSize, resolution)*scale;


	if (!shader)
	{
		shader = new sf::Shader();
		shader->loadFromFile("shaders/grass.vert", "shaders/grass.geom", "shaders/grass.frag");
	}
	shaderUsers++;

}

client::Grass::~Grass()
{
	shaderUsers--;
	if (shaderUsers == 0)
	{
		delete shader;
	}
}

void client::Grass::draw(sf::RenderTarget & target, sf::RenderStates states) const
{

	states.shader = shader;
	shader->setUniform("strength", strength);
	shader->setUniform("size", size);
	states.texture = &texture;
	target.draw(va, states);
}

void client::Grass::update(const sf::Time & time, const sf::Time & dt, Wind & wind)
{
	strength = wind.getValue() * 0.2f;
	strength.y = 1.0f - strength.y;
}
