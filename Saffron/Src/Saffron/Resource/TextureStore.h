#pragma once

#include <SFML/Graphics/Texture.hpp>

#include "Saffron/Interface/IResourceStore.h"
#include "Saffron/Macros/GenericThrowMacros.h"

namespace Se
{
class TextureStore : public IResourceStore<sf::Texture>
{
public:
	TextureStore() = default;
	TextureStore(const TextureStore &) = delete;
	const TextureStore &operator()(const TextureStore &) = delete;

	// Returns pointer resource from cache, if not existing, call Load();
	static sf::Texture *Get(const std::string &filepath)
	{
		if ( _resources.find(filepath) == _resources.end() )
		{
			Load(filepath);
		}
		return &_resources[filepath];
	}
	// Returns copy of resource from cache, if not existing, call Load();
	static const sf::Texture &GetCopy(const std::string &filepath)
	{
		if ( _resources.find(filepath) == _resources.end() )
		{
			Load(filepath);
		}
		return _resources[filepath];
	}
	// Load resource into memory
	static void Load(const std::string &filepath)
	{
		sf::Texture resource;
		if ( !resource.loadFromFile(filepath) )
		{
			{ char buf[200]; sprintf(buf, "Failed to load texture: %s", filepath.c_str()); throw Exception(__LINE__, __FILE__, buf); };
		}
		_resources.emplace(std::make_pair(filepath, resource));
	}
};
}