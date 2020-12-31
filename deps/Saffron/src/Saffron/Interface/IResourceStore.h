#pragma once

#include <map>

#include "Saffron/Interface/IException.h"
#include "Saffron/Macros/GenericThrowMacros.h"

namespace Se
{
template <class T>
class IResourceStore
{
public:
	IResourceStore() = default;

protected:
	static std::map<std::string, T> _resources;

public:
	class Exception : public IException
	{
	public:
		Exception(int line, const char *file, const char *errorString)
			: IException(line, file),
              _errorString(errorString)
		{
		}
		const char *what() const noexcept override
		{
			std::ostringstream oss;
			oss << "[Type] " << GetType() << std::endl
				<< "[Description] " << GetErrorString() << std::endl
				<< GetOriginString();
			whatBuffer = oss.str();
			return whatBuffer.c_str();
		}

		const char *GetType() const override
		{
			static char buffer[100];
            snprintf(buffer, 100, "%s %s", typeid(T).name(), "Exception");
			return buffer;
		}
		const char *GetErrorString() const
		{
			return _errorString.c_str();
		}

	private:
		std::string _errorString;
	};
};

template <typename T>
std::map<std::string, T> IResourceStore<T>::_resources;
}