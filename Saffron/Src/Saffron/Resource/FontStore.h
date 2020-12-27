#pragma once

#include <SFML/Graphics/Font.hpp>

#include "Saffron/Interface/IResourceStore.h"

namespace Se
{
class FontStore : public IResourceStore<sf::Font>
{
public:
	FontStore() = default;
	FontStore(const FontStore &) = delete;
	const FontStore &operator()(const FontStore &) = delete;

	static sf::Font *Get(const std::string &filepath)
	{
		if ( _resources.find(filepath) == _resources.end() )
		{
			Load(filepath);
		}
		return &_resources[filepath];
	}
	// Returns copy of resource from cache, if not existing, call Load();
	static const sf::Font &GetCopy(const std::string &filepath)
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
		sf::Font resource;
		if ( !resource.loadFromFile(filepath) )
		{
			{ char buf[200]; sprintf(buf, "Failed to load font: %s", filepath.c_str()); throw Exception(__LINE__, __FILE__, buf); };
		}
		_resources.emplace(std::make_pair(filepath, resource));
	}
};
}
