#pragma once

#include <SFML/Graphics/Texture.hpp>

#include "Saffron/Resource/ResourceStore.h"

namespace Se
{
class TextureStore : public ResourceStore<sf::Texture>
{
public:
	static Shared<sf::Texture> Get(const Filepath& filepath, bool copy = false)
	{
		return Instance()._Get(filepath, copy);
	}

private:
	Shared<sf::Texture> Copy(const Shared<sf::Texture>& value) override
	{
		return CreateShared<sf::Texture>(*value);
	}

	Filepath GetLocation() override
	{
		return "res/Textures/";
	}

	static TextureStore& Instance()
	{
		static TextureStore resourceStore;
		return resourceStore;
	}

private:
	Shared<sf::Texture> Load(Filepath filepath) override
	{
		auto resource = CreateShared<sf::Texture>();
		const auto result = resource->loadFromFile(filepath.string());
		SE_CORE_ASSERT(result, "Failed to load Texture");
		return resource;
	}
};
}
