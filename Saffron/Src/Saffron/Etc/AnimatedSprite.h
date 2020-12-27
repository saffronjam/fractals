#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

#include "Saffron/Etc/Animation.h"

namespace Se
{
class AnimatedSprite : public sf::Drawable, public sf::Transformable
{
public:
	explicit AnimatedSprite(sf::Time frameTime = sf::seconds(0.2f), bool paused = false, bool looped = true);

	void update(sf::Time deltaTime);
	void setAnimation(const Animation &animation);
	void setFrameTime(sf::Time time);
	void play();
	void play(const Animation &animation);
	void pause();
	void stop();
	void setLooped(bool looped);
	void setColor(const sf::Color &color);
	const Animation *getAnimation() const;
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;
	bool isLooped() const;
	bool isPlaying() const;
	sf::Time getFrameTime() const;
	void setFrame(std::size_t newFrame, bool resetTime = true);

private:
	const Animation *_animation;
	sf::Time _frameTime;
	sf::Time _currentTime;
	std::size_t _currentFrame;
	bool _isPaused;
	bool _isLooped;
	const sf::Texture *_texture;
	sf::Vertex _vertices[4];

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
}