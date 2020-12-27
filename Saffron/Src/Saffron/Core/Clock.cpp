#include "SaffronPCH.h"

#include "Saffron/Core/Clock.h"

namespace Se
{
Clock::Clock()
	: _dt(sf::Time::Zero)
{
}

sf::Time Clock::Restart()
{
	_dt = _clock.restart();
	return _dt;
}
}
