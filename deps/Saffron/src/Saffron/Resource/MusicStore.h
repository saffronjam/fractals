#pragma once

#include <memory>

#include <SFML/Audio/Music.hpp>

#include "Saffron/Interface/IResourceStore.h"

namespace Se
{
class MusicStore : public IResourceStore<std::shared_ptr<sf::Music>>
{
public:
	MusicStore() = default;
	MusicStore(const MusicStore &) = delete;
	const MusicStore &operator()(const MusicStore &) = delete;

public:
	// Returns copy of resource from cache, if not existing, call Load();
	static const std::shared_ptr<sf::Music> &GetCopy(const std::string &filepath)
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
		std::shared_ptr<sf::Music> resource = std::make_shared<sf::Music>();
		if ( !resource->openFromFile(filepath) )
		{
			{ char buf[200]; sprintf(buf, "Failed to open music: %s", filepath.c_str()); throw Exception(__LINE__, __FILE__, buf); };
		}
		_resources.emplace(std::make_pair(filepath, resource));
	}
};
}
