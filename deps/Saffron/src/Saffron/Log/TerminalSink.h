#pragma once

#include "Saffron/Gui/Gui.h"
#include "Saffron/Log/LogSink.h"

namespace Se
{
class TerminalSink : public LogSink
{
public:
	void Clear();

	const ImGuiTextBuffer &GetTextBuffer() const { return _textBuffer; }
	const ArrayList<int> &GetLineOffsets() const { return _lineOffsets; }

protected:
	void Sink(const LogMessage &message) override;
	void Flush() override;

private:
	ImGuiTextBuffer _textBuffer;
	ArrayList<int> _lineOffsets;

};
}