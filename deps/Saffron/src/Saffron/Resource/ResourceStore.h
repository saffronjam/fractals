#pragma once

#include "Saffron/Base.h"
#include "Saffron/Macros/GenericThrowMacros.h"

namespace Se
{
template <class Value>
class ResourceStore
{
public:
	ResourceStore() = default;
	virtual ~ResourceStore() = default;

	Shared<Value> _Get(const Filepath& filepath, bool copy)
	{
		String newFilepath(GetLocation().string() + filepath.string());
		Shared<Value> newResoure;
		if (_resources.find(newFilepath) == _resources.end())
		{
			newResoure = Load(newFilepath);
			_resources.emplace(newFilepath, newResoure);
		}
		else
		{
			newResoure = _resources.at(newFilepath).lock();
		}
		
		if (copy)
		{
			return Copy(newResoure);
		}
		return newResoure;
	}

protected:
	virtual Shared<Value> Load(Filepath filepath)
	{
		SE_CORE_FALSE_ASSERT("Load was called on store that did not implement it");
		return nullptr;
	}

	virtual Shared<Value> Copy(const Shared<Value>& value)
	{
		SE_CORE_FALSE_ASSERT("Copy was called on store that did not implement it");
		return nullptr;
	}

	virtual Filepath GetLocation()
	{
		SE_CORE_FALSE_ASSERT("GetLocation was called on store that did not implement it");
		return {};
	}

private:
	UnorderedMap<String, Weak<Value>> _resources;
};
}
