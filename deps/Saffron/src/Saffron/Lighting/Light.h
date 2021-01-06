#pragma once

#include <utility>
#include <memory>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#include "Saffron/Libraries/GenUtils.h"
#include "Saffron/Libraries/VecUtils.h"

namespace Se
{
class Light
{
	friend class LightningMgr;

public:
	using Ptr = std::shared_ptr<Light>;

protected:
	void BuildBase(const sf::Vector2f &position, float radius, sf::Color color = sf::Color::White, const sf::Vector2f &direction = VecUtils::Null<>(), float theta = 0.0f);

public:
	const sf::Vector2f &GetPosition() const { return _position; }
	float GetRadius() const { return _radius; }
	const sf::Color &GetColor() const { return _color; }
	sf::FloatRect GetBoundingBox() const { return sf::FloatRect(_position.x - GetRadius(), _position.y - GetRadius(), GetRadius() * 2.0f, GetRadius() * 2.0f); }

	void SetPosition(const sf::Vector2f &position) { _position = position; }
	void SetRadius(float radius) { _radius = radius; }
	void SetColor(const sf::Color &color) { _color = color; }

protected:
	void AddStaticOccluder(const sf::Drawable &drawable, const sf::RenderStates &renderStates = sf::RenderStates::Default);
	void ClearStaticOccluders() { _staticOccluderMap.clear(sf::Color(0, 0, 0, 0)); }
	const sf::RenderTexture &GetStaticOccluderMap() const { return _staticOccluderMap; }

protected:
	sf::Vector2f _position;
	float _radius;
	sf::Color _color;
	sf::RenderTexture _staticOccluderMap;
};
}