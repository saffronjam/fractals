#pragma once

#include <SFML/Graphics/Font.hpp>

#include "Saffron/Interface/IResourceMgr.h"

namespace Se
{
class FontMgr : public IResourceMgr<sf::Font>
{
public:
	FontMgr() = default;
	FontMgr(const FontMgr &) = delete;
	const FontMgr &operator()(const FontMgr &) = delete;

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
			THROW(Exception, "Failed to load font: %s", filepath.c_str());
		}
		_resources.emplace(std::make_pair(filepath, resource));
	}
};
}
