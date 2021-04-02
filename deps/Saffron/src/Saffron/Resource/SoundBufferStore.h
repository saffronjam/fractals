#pragma once

#include <SFML/Audio/SoundBuffer.hpp>

#include "Saffron/Resource/ResourceStore.h"

namespace Se
{
class SoundBufferStore : public ResourceStore<sf::SoundBuffer>
{
public:
	static Shared<sf::SoundBuffer> Get(const Filepath& filepath, bool copy)
	{
		return Instance()._Get(filepath, copy);
	}

private:
	Shared<sf::SoundBuffer> Copy(const Shared<sf::SoundBuffer>& value) override
	{
		return CreateShared<sf::SoundBuffer>(*value);
	}

	Filepath GetLocation() override
	{
		return "res/Sounds/";
	}

	static SoundBufferStore& Instance()
	{
		static SoundBufferStore resourceStore;
		return resourceStore;
	}

private:
	Shared<sf::SoundBuffer> Load(Filepath filepath) override
	{
		auto resource = CreateShared<sf::SoundBuffer>();
		const auto result = resource->loadFromFile(filepath.string());
		SE_CORE_ASSERT(result, "Failed to load SoundBuffer");
		return resource;
	}
};
}
