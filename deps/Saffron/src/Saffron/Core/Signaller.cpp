#include "SaffronPCH.h"

#include "Saffron/Core/Signaller.h"

namespace Se
{
Signaller::~Signaller()
{
	_signals.Clear();
}

SignalContainer &Signaller::GetSignals()
{
	return _signals;
}

}