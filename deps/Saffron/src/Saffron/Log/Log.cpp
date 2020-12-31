#include "SaffronPCH.h"

#include "Saffron/Log/Log.h"
#include "Saffron/Log/LogSink.h"

namespace Se {


std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_CoreLogger = spdlog::stdout_color_mt("SAFFRON");
	s_CoreLogger->set_level(spdlog::level::trace);

	s_ClientLogger.reset(new spdlog::logger("APP", {}));
	s_ClientLogger->set_pattern("%^[%H:%M:%S]: %v%$");
}

void Log::AddCoreSink(std::shared_ptr<LogSink> sink)
{
	auto &sinks = const_cast<ArrayList<spdlog::sink_ptr> &>(s_CoreLogger->sinks());
	sinks.push_back(std::dynamic_pointer_cast<spdlog::sinks::sink>(sink));
}


void Log::AddClientSink(std::shared_ptr<LogSink> sink)
{
	auto &sinks = const_cast<ArrayList<spdlog::sink_ptr> &>(s_ClientLogger->sinks());
	sinks.push_back(std::reinterpret_pointer_cast<spdlog::sinks::sink>(sink));
}
}
