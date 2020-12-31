#pragma once

#include <exception>
#include <sstream>

namespace Se
{
class IException : public std::exception
{
public:
	IException(int line, const char *file);
	const char * what() const noexcept override;
	virtual const char *GetType() const;
	int GetLine() const { return _line; }
	const std::string& GetFile() const { return _file; }
	std::string GetOriginString() const;

private:
	int _line;
	std::string _file;

protected:
	mutable std::string whatBuffer;
};
}
