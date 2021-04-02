#pragma once

#include "Saffron/Core/ComputeShader.h"
#include "Saffron/Resource/ResourceStore.h"

namespace Se
{
class ComputeShaderStore : public ResourceStore<ComputeShader>
{
public:
	static Shared<ComputeShader> Get(const Filepath& filepath)
	{
		return Instance()._Get(filepath, false);
	}

private:
	Filepath GetLocation() override
	{
		return "res/Shaders/";
	}

	static ComputeShaderStore& Instance()
	{
		static ComputeShaderStore resourceStore;
		return resourceStore;
	}

private:
	Shared<ComputeShader> Load(Filepath filepath) override
	{
		auto resource = CreateShared<ComputeShader>();
		const auto result = resource->LoadFromFile(filepath.string());
		SE_CORE_ASSERT(result, "Failed to load ComputeShader");
		return resource;
	}
};
}
