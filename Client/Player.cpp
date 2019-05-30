#include "Player.hpp"


#include "../Client/Client.hpp"

template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
	template <typename... Args>
	static Ret callback(Args... args) {
		func(args...);
	}
	static std::function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;

typedef void(*callback_t)(spAnimationState*, spEventType, spTrackEntry*, spEvent*);

void client::Player::callback(spAnimationState * state, spEventType type, spTrackEntry * entry, spEvent * event)
{
	if (ANIMATION_EVENT == type)
	{
		(event->intValue == 0 ? footStepLeft : footStepRight).play();
	}
}

void client::Player::create(ResourceManager & resMg)
{
	footStepLeft.setBuffer(resMg.addSound("sound/player/footstep_grass_r.ogg"));
	footStepRight.setBuffer(resMg.addSound("sound/player/footstep_grass_l.ogg"));
	spAtlas* atlas = resMg.addSpineAtlas("animations/spineboy.atlas");
	spSkeletonData* skeleton = resMg.addSpineSkeleton("animations/spineboy-ess.skel", atlas, 0.25f);

	SkeletonBounds* bounds = spSkeletonBounds_create();

	// Configure mixing.
	AnimationStateData* stateData = spAnimationStateData_create(skeleton);

	AnimationStateData_setMixByName(stateData, "walk", "run", 0.2f);
	AnimationStateData_setMixByName(stateData, "run", "walk", 0.2f);
	AnimationStateData_setMixByName(stateData, "idle", "walk", 0.2f);
	AnimationStateData_setMixByName(stateData, "walk", "idle", 0.2f);
	AnimationStateData_setMixByName(stateData, "run", "idle", 0.2f);
	AnimationStateData_setMixByName(stateData, "idle", "run", 0.2f);

	sprite = std::make_unique<SpineSprite>(skeleton, stateData);

	sprite->skeleton->flipX = false;
	sprite->skeleton->flipY = false;

	sprite->setupPose();
	sprite->setAnimation("idle");

	shadow.setColor(sf::Color(255, 255, 255, 50));


	Callback<void(spAnimationState*, spEventType, spTrackEntry*, spEvent*)>::func = std::bind(&Player::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	callback_t func = static_cast<callback_t>(Callback<void(spAnimationState*, spEventType, spTrackEntry*, spEvent*)>::callback);

	sprite->state->listener = func;

}

client::Player::Player(sf::Vector2f position, Physics & physics, ResourceManager & resMg)
{
	this->create(resMg);
	body = std::make_shared<CircleBody>(25.0f);
	this->body->setType(DynamicBody);
	this->body->setPosition(position);
	physics.addBody(body);
	sf::Texture& st = resMg.addTexture("sprites/shadow/circle.png");
	shadow.setTexture(st);
	shadow.setOrigin(sf::Vector2f(st.getSize()) / 2.0f);

}

void client::Player::setHealth(sf::Int8 health)
{
	this->health = health;
}

sf::Int8 client::Player::getHealth()
{
	return this->health;
}

const void client::Player::setName(std::wstring name)
{
	this->name = name;
}

const std::wstring & client::Player::getName()
{
	return this->name;
}

void client::Player::setAnimation(std::string animation)
{
	if (getAnimation() != animation && !animation.empty())
	{
		sprite->setAnimation(animation);
	}
}

const bool client::Player::isMirrored() const
{
	return sprite->skeleton->flipX;
}

const void client::Player::setMirrored(bool mirrored)
{
	sprite->skeleton->flipX = mirrored;
}

std::string client::Player::getAnimation()
{
	return sprite->getAnimation();
}

const void client::Player::setPosition(sf::Vector2f position)
{
	this->body->setPosition(position*100.f);
	sf::Vector2f finalPos = Math::worldToScreen(position);
	this->screen_y = finalPos.y;
	sprite->setPosition(finalPos);
	shadow.setPosition(finalPos);

}

const sf::Vector2f client::Player::getPosition() const
{
	return this->body->getPosition() / 100.0f;
}

void client::Player::drawPre(sf::RenderTarget & target) const
{
	target.draw(shadow);
}

void client::Player::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(*sprite);
}

void client::LocalPlayer::updateMovement()
{
	this->body->setVelocity(sf::Vector2f());
	bool l = input.get(PlayerAction::MoveLeft);
	bool r = input.get(PlayerAction::MoveRight);
	bool u = input.get(PlayerAction::MoveUp);
	bool d = input.get(PlayerAction::MoveDown);

	this->running = input.get(PlayerAction::Run);

	if (u && d)
	{
		u = false;
		d = false;
	}

	if (l && r)
	{
		l = false;
		r = false;
	}

	if (!u && !d && !r && !l)
	{
		if (sprite->getAnimation() != "idle")
			sprite->setAnimation("idle", true);
		return;
	}
	else if (sprite->getAnimation() != (running ? "run" : "walk"))
	{
		sprite->setAnimation(running ? "run" : "walk", true);
	}
	float angle = 0;

	if (l)
	{
		sprite->skeleton->flipX = true;
		angle = PI;
		if (u)
		{
			angle += Math::QUARTER_PI;
		}
		else if (d)
		{
			angle -= Math::QUARTER_PI;
		}
	}
	else if (r)
	{
		sprite->skeleton->flipX = false;
		angle = 0.0f;
		if (u)
		{
			angle -= Math::QUARTER_PI;
		}
		else if (d)
		{
			angle += Math::QUARTER_PI;
		}
	}
	else if (u)
	{
		angle -= Math::HALF_PI;
	}
	else if (d)
	{
		angle += Math::HALF_PI;
	}

	angle -= Math::QUARTER_PI;
	this->body->setVelocity((running ? 200.f : 70.f)*sf::Vector2f(std::sin(angle), std::cos(angle)));
}

client::LocalPlayer::LocalPlayer(sf::Vector2f position, Physics & physics, Input & input, sf::View & view, ResourceManager & resMg) : input(input), view(view), Player(position, physics, resMg)
{

}

void client::LocalPlayer::setHealth(sf::Int8 health)
{
	this->Player::setHealth(health);
}


sf::Vector2f client::LocalPlayer::getPosition()
{
	return this->body->getPosition() / 100.0f;
}

void client::LocalPlayer::drawGUI(sf::RenderTarget & target)
{

}

void client::LocalPlayer::update(const sf::Time & time, const sf::Time & dt, Wind & wind)
{
	updateMovement();
	setPosition(this->body->getPosition() / 100.0f);
	const float fdt = static_cast<float>(dt.asMicroseconds()) / 1000000.0f;


	AABB = sf::FloatRect(sf::Vector2f(sprite->skeleton->x - 75, sprite->skeleton->y - 200), sf::Vector2f(150, 200));

	sprite->update(fdt);

	this->view.move((sf::Vector2f(sprite->skeleton->x, sprite->skeleton->y - 100.0f) - this->view.getCenter()) * 2.0f * fdt);
}
