#include "SaffronPCH.h"

#include "Saffron/Physics/Collider.h"
#include "Saffron/Physics/PhysicsMgr.h"

namespace Se
{
void Collider::BuildBase(b2BodyDef *bodyDef, b2FixtureDef *fixtureDef)
{
	PhysicsMgr::AddCollider(this, bodyDef);
	_fixture = _body.value()->CreateFixture(fixtureDef);
}
}