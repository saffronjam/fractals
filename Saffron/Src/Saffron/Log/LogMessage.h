#pragma once

#include "Saffron/Log/Log.h"

namespace Se
{
struct LogMessage
{
	explicit LogMessage(const spdlog::details::log_msg &message)
		:
		Raw(message.raw.c_str()),
		Formatted(message.formatted.c_str()),
		Level(static_cast<Log::Level::LevelEnum>(message.level))
	{
	}

	String Raw;
	String Formatted;
	Log::Level::LevelEnum Level;
};
}