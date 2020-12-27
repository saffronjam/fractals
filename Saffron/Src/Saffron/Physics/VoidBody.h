#pragma once

#include <box2d/box2d.h>
#include <SFML/System/Vector2.hpp>

#include "Saffron/Physics/PhysicsMgr.h"

namespace Se
{
class VoidBody : public Collider
{
public:
	using Ptr = std::shared_ptr<VoidBody>;

public:
	static Ptr Create(const sf::Vector2f &position);

private:
};
}
