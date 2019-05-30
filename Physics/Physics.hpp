#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <memory>
#include "../System/Math.hpp"
#include "../Physics/BodyBase.hpp"
#include "../Physics/CircleBody.hpp"
#include "../Physics/RectBody.hpp"

class Physics
{
private:
	std::vector<std::shared_ptr<Body>> bodies;
	const float fix = 1.005f;

	void circleCircleCollision(const std::shared_ptr<CircleBody>& c1, const std::shared_ptr<CircleBody>& c2);
	void rectRectCollision(const std::shared_ptr<RectBody>& r1, const std::shared_ptr<RectBody>& r2);
	void circleRectCollision(const std::shared_ptr<CircleBody> circle, const std::shared_ptr<RectBody> rect);
	void handleCollision(std::shared_ptr<Body>& b1, std::shared_ptr<Body>& b2);
public:
	void update(sf::Time& dt);
	void addBody(std::shared_ptr<Body> body);
};

