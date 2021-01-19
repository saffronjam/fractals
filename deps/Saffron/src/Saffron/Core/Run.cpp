#include "SaffronPCH.h"

#include "Saffron/Core/Global.h"
#include "Saffron/Core/Run.h"

namespace Se
{
ArrayList<Function<void()>> Run::_laterFunctions;
Map<Run::Handle, Run::PeriodicFunction> Run::_periodicFunctions;
Map<Run::Handle, Function<void()>> Run::_frameFunctions;

void Run::Execute()
{
	const auto ts = Global::Clock::GetFrameTime();

	for ( auto &function : _laterFunctions )
	{
		function();
	}
	_laterFunctions.clear();

	for ( auto &[handle, periodicFunction] : _periodicFunctions )
	{
		periodicFunction.currentCounter += ts;
		if ( periodicFunction.currentCounter >= periodicFunction.interval )
		{
			periodicFunction.currentCounter = sf::Time::Zero;
			periodicFunction();
		}
	}

	for ( auto &[handle, frameFunction] : _frameFunctions )
	{
		frameFunction();
	}
}

void Run::Later(Function<void()> function)
{
	_laterFunctions.push_back(Move(function));
}

Run::Handle Run::Periodically(Function<void()> function, sf::Time interval)
{
	Handle newHandle;
	_periodicFunctions.emplace(newHandle, PeriodicFunction{ Move(function), interval, sf::Time::Zero });
	return newHandle;
}

Run::Handle Run::EveryFrame(Function<void()> function)
{
	Handle newHandle;
	_frameFunctions.emplace(newHandle, Move(function));
	return newHandle;
}

void Run::Remove(Handle handle)
{
	_periodicFunctions.erase(handle);
	_frameFunctions.erase(handle);
}
}
