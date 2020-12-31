#include "SaffronPCH.h"


#include <SFML/System/Sleep.hpp>

#include "Saffron/Core/Clock.h"

#include "Saffron/Core/FpsLimiter.h"

using namespace std::chrono;

namespace Se
{
float FpsLimiter::_desiredFps = 0.0f;
steady_clock::time_point FpsLimiter::_last;

void FpsLimiter::Start()
{
	_last = steady_clock::now();
}

void FpsLimiter::Adjust()
{
	if ( _desiredFps != 0.0f )
	{
		const auto old = _last;
		_last = steady_clock::now();

		const duration<float> chronoFrameTime = _last - old;
		const float frameTime = chronoFrameTime.count();
		const float secondsRemaining = (1.0f / _desiredFps) - frameTime;

		sf::sleep(sf::seconds(secondsRemaining));
	}
}
}