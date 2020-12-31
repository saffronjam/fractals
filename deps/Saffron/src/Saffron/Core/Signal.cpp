#include "SaffronPCH.h"

#include "Saffron/Core/Signal.h"

namespace Se
{

Uint32 Signal<void>::_serial = 0;

//////////////////////////////////////////////////////
/// Basic Signal
//////////////////////////////////////////////////////

BasicSignal::TypeID BasicSignal::GenerateTypeID()
{
	return {};
}

void SignalContainer::Clear()
{
	if ( _signals )
	{
		_signals->clear();
		_signals.reset();
	}
}
}
