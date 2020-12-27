#include "SaffronPCH.h"

#include "Saffron/Physics/CircleCollider.h"

namespace Se
{
CircleCollider::Ptr CircleCollider::Create(const sf::Vector2f &position, float radius)
{
	Ptr ret = std::make_shared<CircleCollider>();
	ret->_radius = radius;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x, position.y);

	b2CircleShape shape;
	shape.m_radius = ret->_radius;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	ret->BuildBase(&bodyDef, &fixtureDef);

	return ret;
}
}
