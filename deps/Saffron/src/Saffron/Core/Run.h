#pragma once

#include <SFML/System/Time.hpp>

#include "Saffron/Core/UUID.h"

namespace Se
{
class Run
{
public:
	struct Handle
	{
		bool operator<(const Handle &other) const { return _uuid < other._uuid; }
		bool operator==(const Handle &other) const { return _uuid == other._uuid; }
	private:
		UUID _uuid;
	};

private:
	struct PeriodicFunction
	{
		void operator()() { function(); }
		void operator()() const { function(); }

		Function<void()> function;
		sf::Time interval;
		sf::Time currentCounter;
	};

public:
	static void Execute();

	static void Later(Function<void()> function);
	static Handle Periodically(Function<void()> function, sf::Time interval);
	static Handle EveryFrame(Function<void()> function);

	static void Remove(Handle handle);

private:
	static ArrayList<Function<void()>> _laterFunctions;
	static Map<Handle, PeriodicFunction> _periodicFunctions;
	static Map<Handle, Function<void()>> _frameFunctions;

};
}
