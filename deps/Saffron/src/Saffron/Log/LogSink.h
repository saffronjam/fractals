#pragma once

#include "Saffron/Base.h"
#include "Saffron/Log/Log.h"
#include "Saffron/Log/LogMessage.h"
#include "Saffron/Core/ScopedLock.h"

namespace Se
{
class LogSink : public spdlog::sinks::sink
{
	friend class Log;

public:
	virtual void Sink(const LogMessage &message) = 0;
	virtual void Flush() = 0;

	void SetLevel(Log::Level::LevelEnum level);

protected:
	void log(const spdlog::details::log_msg &msg) final override
	{
		ScopedLock lock(_mutex);
		Sink(LogMessage(msg));
	}

	void flush() final override
	{
		ScopedLock lock(_mutex);
		Flush();
	}

private:
	Mutex _mutex;
};
}