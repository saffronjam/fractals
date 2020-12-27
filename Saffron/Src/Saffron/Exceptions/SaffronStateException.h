#pragma once

#include <exception>

#include "Saffron/Base.h"

namespace Se
{
class SaffronStateException : public std::exception
{
public:
	explicit SaffronStateException(const String &msg)
		: exception(msg.c_str())
	{
	}
};
}
