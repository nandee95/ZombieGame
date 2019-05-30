#include "Physics.hpp"

void Physics::circleCircleCollision(const std::shared_ptr<CircleBody>& c1, const std::shared_ptr<CircleBody>& c2)
{
	const float dist = Math::distance(c1->_nextPosition, c2->_nextPosition);
	const float sumRadius = c1->getRadius() + c2->getRadius();
	if (dist < sumRadius)
	{
		if (c1->getType() == DynamicBody)
		{
			c1->_nextPosition = c1->_nextPosition + Math::normalize(c1->_nextPosition - c2->_nextPosition)*(sumRadius - dist)*fix;
		}
		if (c2->getType() == DynamicBody)
		{
			c2->_nextPosition = c2->_nextPosition + Math::normalize(c2->_nextPosition - c1->_nextPosition)*(sumRadius - dist)*fix;
		}
	}
}

void Physics::rectRectCollision(const std::shared_ptr<RectBody>& r1, const std::shared_ptr<RectBody>& r2)
{
	const sf::Vector2f sumSize = r1->getSize() + r2->getSize();
	const sf::Vector2f diff = r1->_nextPosition - r2->_nextPosition;
	if (sumSize.x > std::abs(diff.x) &&
		sumSize.y > std::abs(diff.y))
	{
		const sf::Vector2f test = Math::vec_div(sf::Vector2f(diff.x, diff.y), sumSize);
		if (std::abs(test.x) > std::abs(test.y))
		{
			if (r1->getType() == DynamicBody)
			{
				r1->_nextPosition.x = r2->_nextPosition.x + Math::sign(diff.x)*sumSize.x * fix;
			}

			if (r2->getType() == DynamicBody)
			{
				r2->_nextPosition.x = r1->_nextPosition.x + Math::sign(diff.x)*sumSize.x * fix;
			}
		}
		else
		{
			if (r1->getType() == DynamicBody)
			{
				r1->_nextPosition.y = r2->_nextPosition.y + Math::sign(diff.y)*sumSize.y * fix;
			}

			if (r2->getType() == DynamicBody)
			{
				r2->_nextPosition.y = r1->_nextPosition.y + Math::sign(diff.y)*sumSize.y * fix;
			}
		}
	}
}

void Physics::circleRectCollision(const std::shared_ptr<CircleBody> circle, const std::shared_ptr<RectBody> rect)
{
	const sf::Vector2f diff = circle->_nextPosition - rect->_nextPosition;
	const float& radius = circle->getRadius();
	const sf::Vector2f& rsize = rect->getSize();
	const sf::Vector2f peneration = rsize + sf::Vector2f(radius, radius);

	if (std::abs(diff.x) < peneration.x&&std::abs(diff.y) < peneration.y)
	{
		const sf::Vector2f test = Math::vec_div(sf::Vector2f(std::abs(diff.x), std::abs(diff.y)), peneration);
		if (std::abs(test.x) > std::abs(test.y))
		{
			if (std::abs(diff.y) < rsize.y)
			{
				if (circle->getType() == DynamicBody)
				{
					circle->_nextPosition.x = rect->_nextPosition.x + Math::sign(diff.x)*(rsize.x + radius)* fix;
				}
				if (rect->getType() == DynamicBody)
				{
					rect->_nextPosition.x = circle->_nextPosition.x - Math::sign(diff.x)*(rsize.x + radius)* fix;
				}
			}
			else goto corner;
		}
		else
		{
			if (std::abs(diff.x) < rsize.x)
			{
				if (circle->getType() == DynamicBody)
				{
					circle->_nextPosition.y = rect->_nextPosition.y + Math::sign(diff.y)*(rsize.y + radius)* fix;
				}
				if (rect->getType() == DynamicBody)
				{
					rect->_nextPosition.y = circle->_nextPosition.y - Math::sign(diff.y)*(rsize.y + radius)* fix;
				}
			}
			else goto corner;
		}
		return;
	corner:
		const sf::Vector2f corner = rect->_nextPosition + Math::vec_mul(rsize, sf::Vector2f(Math::sign(diff.x), Math::sign(diff.y)));
		if (Math::distance(corner, circle->_nextPosition) < radius)
		{
			const sf::Vector2f& pcircle = circle->_nextPosition;
			const sf::Vector2f direction = Math::normalize(pcircle - corner);
			if (circle->getType() == DynamicBody)
			{
				circle->_nextPosition = corner + direction * radius * fix;
			}
			if (rect->getType() == DynamicBody)
			{
				rect->_nextPosition = rect->getPosition() - direction * (radius - Math::distance(corner, circle->getPosition()))*fix;
			}
		}
		return;
	}
}

void Physics::handleCollision(std::shared_ptr<Body>& b1, std::shared_ptr<Body>& b2)
{
	const std::shared_ptr<CircleBody> c1 = std::dynamic_pointer_cast<CircleBody>(b1);
	const std::shared_ptr<CircleBody> c2 = std::dynamic_pointer_cast<CircleBody>(b2);
	const std::shared_ptr<RectBody> r1 = std::dynamic_pointer_cast<RectBody>(b1);
	const std::shared_ptr<RectBody> r2 = std::dynamic_pointer_cast<RectBody>(b2);
	if (c1  && c2)
	{
		circleCircleCollision(c1, c2);
		return;
	}
	if (r1  && r2)
	{
		rectRectCollision(r1, r2);
		return;
	}
	if ((c1 && r2) || (c2 && r1))
	{
		circleRectCollision(c1 ? c1 : c2, r1 ? r1 : r2);
	}
}

void Physics::update(sf::Time& dt)
{
	for (auto& b : bodies)
	{
		if (b->getType() == StaticBody) continue;
		b->_nextPosition = b->getPosition() + (b->getVelocity()*static_cast<float>(dt.asMicroseconds()/1000000.0f));
	}

	for (auto& b1 : bodies)
	{
		if (b1->getType() == StaticBody) continue;
		for (auto& b2 : bodies)
		{
			if (b1 == b2 || (b1->_nextPosition == b1->position && b2->_nextPosition == b2->position)) continue;
			if (b2->getType() == DynamicBody)
				handleCollision(b1, b2);
		}
		for (auto& b2 : bodies)
		{
			if (b1 == b2 || (b1->_nextPosition == b1->position && b2->_nextPosition == b2->position)) continue;
			if (b2->getType() == StaticBody)
				handleCollision(b1, b2);
		}
	}

	for (auto& b : bodies)
	{
		if (b->getType() == StaticBody) continue;
		b->setPosition(b->_nextPosition);
	};
}

void Physics::addBody(std::shared_ptr<Body> body)
{
	this->bodies.push_back(body);
}
