#pragma once

#include <SFML/Graphics/Image.hpp>

#include "Saffron/Resource/ResourceStore.h"

namespace Se
{
class ImageStore : public ResourceStore<sf::Image>
{
public:
	static Shared<sf::Image> Get(const Filepath& filepath, bool copy = false)
	{
		return Instance()._Get(filepath, copy);
	}

private:	
	Shared<sf::Image> Copy(const Shared<sf::Image>& value) override
	{
		auto resource = CreateShared<sf::Image>();
		resource->copy(*value, 0, 0);
		return resource;
	}

	Filepath GetLocation() override
	{
		return "res/Textures/";
	}

	static ImageStore& Instance()
	{
		static ImageStore resourceStore;
		return resourceStore;
	}

private:
	Shared<sf::Image> Load(Filepath filepath) override
	{
		auto resource = CreateShared<sf::Image>();
		const auto result = resource->loadFromFile(filepath.string());
		SE_CORE_ASSERT(result, "Failed to load Image");
		return resource;
	}
};
}
