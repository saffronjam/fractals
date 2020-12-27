#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "Saffron/Base.h"
#include "Saffron/Physics/PhysicsMgr.h"

namespace Se
{
class BoxCollider : public Collider
{
public:
	using Ptr = std::shared_ptr<BoxCollider>;

public:
	static Ptr Create(const sf::Vector2f &position, const sf::Vector2f &dimensions);

	const sf::Vector2f &GetDimensions() const { return _dimensions; }
	sf::FloatRect GetRect() const;

private:
	sf::Vector2f _dimensions;
};
}