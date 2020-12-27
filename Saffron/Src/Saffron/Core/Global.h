#pragma once

#include "Saffron/Core/Clock.h"

namespace Se
{
class Global
{
public:
	class Clock
	{
	public:
		using LocalClock = Se::Clock;

		static sf::Time GetFrameTime() { return _localClock.GetFrameTime(); }
		static sf::Time GetElapsedTime() { return _localClock.GetElapsedTime(); }
		static sf::Time Restart() { return _localClock.Restart(); }

	private:
		static LocalClock _localClock;
	};
};
}
