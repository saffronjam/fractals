#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "Saffron/Base.h"

namespace Se
{
class ControllableRenderTexture
{
public:
	ControllableRenderTexture(int width, int height, sf::ContextSettings contextSettings = sf::ContextSettings());

	operator sf::RenderTexture &() { return _renderTexture; }
	operator const sf::RenderTexture &() const { return _renderTexture; }
	operator const sf::Texture &() const { return _renderTexture.getTexture(); }

	sf::RenderTexture &GetRenderTexture() { return _renderTexture; }
	const sf::RenderTexture &GetRenderTexture() const { return _renderTexture; }

	bool IsEnabled() const { return _enabled; }
	void Enable() { _enabled = true; }
	void Disable() { _enabled = false; }

	sf::Color GetClearColor() const { return _clearColor; }
	void SetClearColor(sf::Color color) { _clearColor = color; }

private:
	sf::RenderTexture _renderTexture;
	bool _enabled = true;
	sf::Color _clearColor = sf::Color::Black;
};
}
