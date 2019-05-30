#include "Debug.hpp"

DebugOverlay::DebugOverlay(ResourceManager & resMg, sf::View & view) : view(view)
{
	vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	info.setPosition(100, 100);
	info.setFont(resMg.addFont("fonts/arial.ttf"));
	info.setOutlineThickness(1.0f);
	info.setOutlineColor(sf::Color::Black);

}

bool DebugOverlay::chunkIntersectsScreen(sf::Vector2i chunk, sf::FloatRect screen)
{
	if (sf::FloatRect(Math::worldToScreen(sf::Vector2f(chunk)*10.0f) - sf::Vector2f(-640.0f, 0.0f), sf::Vector2f(2560.0f, 1280.0f)).intersects(screen))
	{
		return true;
	}
	return false;
}

void DebugOverlay::update(sf::Time dt, sf::Vector2f playerPosition, Wind & wind, int entityCount, int totalEntities)
{
	float fps = 1.0f / (static_cast<float>(dt.asMicroseconds()) / 1000000.0f);


	std::string infotext = "Player info:\n";
	infotext += "	Position X: " + std::to_string(playerPosition.x) + " ; " + std::to_string(playerPosition.y) + "\n";
	infotext += "Wind:\n";
	infotext += "	Angle: " + std::to_string(static_cast<int>(Math::rad_to_deg(wind.getAngle()))) + "°\n";
	infotext += "	Strength: " + std::to_string(static_cast<int>(wind.getStrength()*100.0f)) + "%\n";
	infotext += "Rendering:\n";
	infotext += "	Entites: " + std::to_string(entityCount) + "/" + std::to_string(totalEntities) + "\n";
	infotext += "	FPS: " + std::to_string(fps) + "\n";
	infotext += "Config:\n";
	infotext += "	GPU: " + vendor + " " + renderer + "\n";
	this->info.setString(infotext);
}

void DebugOverlay::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(info);
}
