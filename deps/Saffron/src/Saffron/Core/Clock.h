#pragma once

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

namespace Se
{
class Clock
{
public:
	Clock();

	sf::Time GetFrameTime() const { return _dt; }
	sf::Time GetElapsedTime() const { return _clock.getElapsedTime(); }
	sf::Time Restart();

private:
	sf::Clock _clock;
	sf::Time _dt;
};
}
