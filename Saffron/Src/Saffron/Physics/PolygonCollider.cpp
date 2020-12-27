#include "SaffronPCH.h"

#include "Saffron/Physics/PolygonCollider.h"

namespace Se
{
PolygonCollider::Ptr PolygonCollider::Create(const sf::Vector2f &position, std::vector<sf::Vector2f> points)
{
	Ptr ret = std::make_shared<PolygonCollider>();

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x, position.y);

	b2PolygonShape shape;
	shape.Set(reinterpret_cast<b2Vec2 *>(points.data()), points.size());

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	ret->BuildBase(&bodyDef, &fixtureDef);

	return ret;
}
}
