#pragma once

#include <SFML/Audio/Music.hpp>

#include "Saffron/Resource/ResourceStore.h"

namespace Se
{
class MusicStore : public ResourceStore<sf::Music>
{
public:
	static Shared<sf::Music> Get(const Filepath& filepath)
	{
		return Instance()._Get(filepath, false);
	}

private:
	Filepath GetLocation() override
	{
		return "res/Sounds/";
	}

	static MusicStore& Instance()
	{
		static MusicStore resourceStore;
		return resourceStore;
	}

private:
	Shared<sf::Music> Load(Filepath filepath) override
	{
		auto resource = CreateShared<sf::Music>();
		const auto result = resource->openFromFile(filepath.string());
		SE_CORE_ASSERT(result, "Failed to load Music");
		return resource;
	}
};
}
