#include "SaffronPCH.h"

#include "Saffron/Log/TerminalSink.h"

namespace Se
{
void TerminalSink::Clear()
{
	_textBuffer.clear();
	_lineOffsets.clear();
	_lineOffsets.push_back(0);
}

void TerminalSink::Sink(const LogMessage &message)
{
	int old_size = _textBuffer.size();
	_textBuffer.append(message.Formatted.c_str());
	for ( const int new_size = _textBuffer.size(); old_size < new_size; old_size++ )
		if ( _textBuffer[old_size] == '\n' )
			_lineOffsets.push_back(old_size + 1);
}

void TerminalSink::Flush()
{
}
}
