#pragma once

#include <box2d/box2d.h>
#include <SFML/System/Vector2.hpp>

#include "Saffron/Base.h"
#include "Saffron/Physics/PhysicsMgr.h"

namespace Se
{
class CircleCollider : public Collider
{
public:
	using Ptr = std::shared_ptr<CircleCollider>;

public:
	static Ptr Create(const sf::Vector2f &position, float radius);

	float GetRadius() const { return _radius; }

private:
	float _radius;
};
}