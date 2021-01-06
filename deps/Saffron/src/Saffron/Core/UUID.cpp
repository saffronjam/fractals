#include "SaffronPCH.h"

#include "Saffron/Core/UUID.h"
#include "Saffron/Math/Random.h"

namespace Se
{

static RandomGenerator<Uint64> sRandom(0, std::numeric_limits<Uint64>::max());


UUID::UUID()
	: _uuid(sRandom.Generate())
{
}

UUID::UUID(Uint64 uuid)
	: _uuid(uuid)
{
}

UUID::UUID(const UUID &other)
	: UUID(other._uuid)
{
}

}