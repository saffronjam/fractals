#pragma once

#include "Saffron/Log/TerminalSink.h"

namespace Se
{
class Terminal
{
public:
	Terminal();

	void Clear();
	void OnGuiRender();

	void SetLevel(Log::Level::LevelEnum level);

private:
	std::shared_ptr<TerminalSink> _sink;
	ImGuiTextFilter _filter;
	bool _autoScroll = false;
};
}