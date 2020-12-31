#include "SaffronPCH.h"

#include "Saffron/Interface/IException.h"

namespace Se
{
IException::IException(int line, const char *file)
	: _line(line), _file(file)
{
}

const char *IException::what() const noexcept
{
	std::ostringstream oss;
	oss << "[Type] " << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char *IException::GetType() const
{
	return "IException";
}

std::string IException::GetOriginString() const
{
	std::ostringstream oss;
	oss << "[File] " << _file << std::endl
		<< "[Line] " << _line << std::endl;
	return oss.str();
}
}