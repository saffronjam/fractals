#pragma once

#include "Saffron/Core/Signal.h"

namespace Se
{
class Signaller
{
public:
	Signaller() = default;
	virtual ~Signaller();

	Signaller(Signaller &&) = default;
	Signaller &operator=(Signaller &&) = default;

	/** Request a reference to a specific signal for this object.
	 * @param signalAggregate
	 * @return Reference to requested signal.
	 */
	template<typename t_ParamType>
	Signal<t_ParamType> &GetSignal(SignalAggregate<t_ParamType> &signalAggregate)
	{
		if ( !signalAggregate.TypeID )
		{
			signalAggregate.TypeID = Signal<t_ParamType>::GenerateTypeID();
		}

		auto *dyncastSignal = dynamic_cast<Signal<t_ParamType> *>(&_signals[signalAggregate]);

		SE_CORE_ASSERT(dyncastSignal);

		return *dyncastSignal;
	}

protected:
	/** Get the signal container.
	 * @return Reference to the signal container.
	 */
	SignalContainer &GetSignals();

private:
	SignalContainer _signals;
};
}
