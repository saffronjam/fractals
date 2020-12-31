#pragma once

#undef UUID_DEFINED

#include "Saffron/Core/TypeDefs.h"

namespace Se
{
class UUID
{
public:
	UUID();
	UUID(Uint64 uuid);
	UUID(const UUID &other);

	operator Uint64 () { return _uuid; }
	operator const Uint64() const { return _uuid; }

	bool operator==(const UUID &other) const
	{
		return _uuid == other._uuid;
	}

	static UUID Null() { return UUID(0ull); }

private:
	Uint64 _uuid;
};
}

namespace std
{
template <>
struct hash<Se::UUID>
{
	std::size_t operator()(const Se::UUID &uuid) const
	{
		return hash<Se::Uint64>()(static_cast<Se::Uint64>(uuid));
	}
};
}

