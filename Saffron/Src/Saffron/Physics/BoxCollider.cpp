#include "SaffronPCH.h"

#include "Saffron/Physics/BoxCollider.h"

namespace Se
{
BoxCollider::Ptr BoxCollider::Create(const sf::Vector2f &position, const sf::Vector2f &dimensions)
{
	Ptr ret = std::make_shared<BoxCollider>();
	ret->_dimensions = dimensions;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x + dimensions.x / 2.0f, position.y + dimensions.y / 2.0f);

	b2PolygonShape shape;
	shape.SetAsBox(ret->_dimensions.x / 2.0, ret->_dimensions.y / 2.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	ret->BuildBase(&bodyDef, &fixtureDef);

	return ret;
}

sf::FloatRect BoxCollider::GetRect() const
{
	const sf::Vector2f topLeft = vl::ConvertTo<sf::Vector2f>(GetBody()->GetPosition());
	return sf::FloatRect(topLeft, _dimensions);
}
}
