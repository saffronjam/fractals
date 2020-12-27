#include "SaffronPCH.h"

#include "Saffron/Physics/VoidBody.h"

namespace Se
{
VoidBody::Ptr VoidBody::Create(const sf::Vector2f &position)
{
	Ptr ret = std::make_shared<VoidBody>();

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x, position.y);

	b2CircleShape shape;
	shape.m_radius = 0.0f;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;

	ret->BuildBase(&bodyDef, &fixtureDef);

	return ret;
}
}
