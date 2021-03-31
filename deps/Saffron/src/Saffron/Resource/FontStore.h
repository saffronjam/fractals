#pragma once

#include <SFML/Graphics/Font.hpp>

#include "Saffron/Resource/ResourceStore.h"

namespace Se
{
class FontStore : public ResourceStore<sf::Font>
{
public:
	static Shared<sf::Font> Get(const Filepath& filepath, bool copy = false)
	{
		return Instance()._Get(filepath, copy);
	}

private:
	Shared<sf::Font> Copy(const Shared<sf::Font>& value) override
	{
		return CreateShared<sf::Font>(*value);
	}

	Filepath GetLocation() override
	{
		return "res/Fonts/";
	}

	static FontStore& Instance()
	{
		static FontStore resourceStore;
		return resourceStore;
	}

private:
	Shared<sf::Font> Load(Filepath filepath) override
	{
		auto resource = CreateShared<sf::Font>();
		const auto result = resource->loadFromFile(filepath.string());
		SE_CORE_ASSERT(result, "Failed to load Font");
		return resource;
	}
};
}
