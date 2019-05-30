#pragma once

#include <string>
#include <SFML\Graphics.hpp>
#include "../Client/Level.hpp"
#include "../Client/Input.hpp"
#include "../Physics/Physics.hpp"
#include "../spine-sfml.h"
#include "../Entities/Entity.hpp"

using namespace spine;

namespace client
{
	enum PlayerAction : char
	{
		MoveUp,
		MoveDown,
		MoveLeft,
		MoveRight,
		Run
	};

	class Player : public Entity
	{
	protected:

		std::shared_ptr<CircleBody> body;
		const float speed = 2.2f;
		const float jumpforce = 0.5f;
		const float moveVertical = 0.1f;

		sf::Int8 health = 100;

		std::wstring name;

		std::unique_ptr<SpineSprite> sprite;

		sf::Sprite shadow;

		bool running = false;

		sf::Sound footStepLeft, footStepRight;

		void callback(spAnimationState* state, spEventType type, spTrackEntry* entry, spEvent* event);

		void create(ResourceManager& resMg);

	public:
		Player(sf::Vector2f position, Physics& physics, ResourceManager& resMg);


		void setHealth(sf::Int8 health);

		sf::Int8 getHealth();

		const void setName(std::wstring name);

		const std::wstring& getName();

		void setAnimation(std::string animation);

		const bool isMirrored() const;

		const void setMirrored(bool mirrored);

		std::string getAnimation();

		const void setPosition(sf::Vector2f position);

		const sf::Vector2f getPosition() const;

		virtual void drawPre(sf::RenderTarget& target) const;

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	};

	class LocalPlayer : public Player
	{
	private:
		float displayHealth = static_cast<float>(this->health);
		Input input;
		bool lastJump = false;
		sf::View& view;

		void updateMovement();

	public:
		LocalPlayer(sf::Vector2f position, Physics& physics, Input& input, sf::View& view, ResourceManager& resMg);

		void setHealth(sf::Int8 health);

		void event(sf::Event& e)
		{
			this->input.event(e);
		}

		sf::Vector2f getPosition();

		void drawGUI(sf::RenderTarget& target);

		virtual void update(const sf::Time& time, const sf::Time& dt, Wind& wind);
	};

	class RemotePlayer : public Player
	{
	private:
		const float interPolationFactor = 0.0f;
		sf::Vector2f targetPosition;
		sf::Vector2f startPosition;
		sf::Time startTime, lastServerFrame, duration;
		sf::Text nameText;
		float distance;
	public:
		RemotePlayer(sf::Vector2f position, Physics& physics, ResourceManager& resMg) : Player(position, physics, resMg)
		{
			this->body->setType(BodyType::StaticBody);
			nameText.setFillColor(sf::Color(255, 255, 255, 150));
			nameText.setFont(resMg.addFont("fonts/arial.ttf"));
			nameText.setOutlineThickness(1.0f);
			nameText.setOutlineColor(sf::Color(0, 0, 0, 150));
		}
		const void setPositionInterpolation(sf::Time& time, sf::Vector2f position)
		{
			this->startPosition = getPosition();
			this->targetPosition = position;
			this->duration = time - lastServerFrame;
			this->startTime = time;
			this->lastServerFrame = time;
		}

		virtual void update(const sf::Time& time, const sf::Time& dt, Wind& wind)
		{
			const float fdt = static_cast<float>(dt.asMicroseconds()) / 1000000.0f;
			const float percent = static_cast<float>((time - startTime).asMicroseconds()) / static_cast<float>(duration.asMicroseconds());
			sprite->update(fdt);
			setPosition(startPosition + (this->targetPosition - startPosition) * percent);
			this->nameText.setPosition(sf::Vector2f(sprite->skeleton->x, sprite->skeleton->y) + sf::Vector2f(0, -210));
		}

		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
		{
			this->Player::draw(target, states);
			target.draw(nameText);
		}
		const void setName(std::wstring name)
		{
			this->name = name;
			this->nameText.setString(name);
			this->nameText.setOrigin(sf::Vector2f(this->nameText.getLocalBounds().width, this->nameText.getLocalBounds().height) / 2.0f);
		}
	};

};