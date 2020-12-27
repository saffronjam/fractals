#pragma once

#include <vector>

#include <box2d/box2d.h>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

#include "Saffron/Physics/Collider.h"
#include "Saffron/Core/Clock.h"
#include "Saffron/Libraries/VecUtils.h"

namespace Se
{
class PhysicsMgr : public sf::NonCopyable
{
public:
	PhysicsMgr();

	static void Step();

	static void AddCollider(Collider *collider, const b2BodyDef *bodyDef);
	static b2World *GetWorld() { return _world.get(); }

private:
	static std::unique_ptr<b2World> _world;
	static std::vector<Collider::Ptr> _colliders;
};
}
