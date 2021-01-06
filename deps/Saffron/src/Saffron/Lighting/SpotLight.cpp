#include "SaffronPCH.h"

#include "Saffron/Lighting/SpotLight.h"

namespace Se
{
SpotLight::Ptr
SpotLight::Create(const sf::Vector2f &position, const sf::Vector2f &direction, float theta, float radius,
				  sf::Color color)
{
	Ptr ret = std::make_shared<SpotLight>();
	ret->BuildBase(position, radius, color, direction, theta);
	ret->SetDirection(direction, theta);
	return ret;
}

void SpotLight::SetDirection(const sf::Vector2f &newDirection, float theta)
{
	ClearStaticOccluders();

	const sf::Vector2f unitDirection = VecUtils::Unit(newDirection);
	sf::VertexArray shape(sf::TriangleFan);
	const sf::Vector2f adjustedPosition = _position + sf::Vector2f(_radius, _radius);
	shape.append(sf::Vertex(adjustedPosition - unitDirection * 10.0f, sf::Color::Black));
	shape.append(sf::Vertex(
		adjustedPosition + VecUtils::Rotate(unitDirection, GenUtils::ToRadians(theta / 2.0f), VecUtils::Null<>()) * 50.0f,
		sf::Color::Black));
	shape.append(sf::Vertex(adjustedPosition - unitDirection * 25.0f, sf::Color::Black));
	shape.append(sf::Vertex(
		adjustedPosition + VecUtils::Rotate(unitDirection, GenUtils::ToRadians(-theta / 2.0f), VecUtils::Null<>()) * 50.0f,
		sf::Color::Black));

	AddStaticOccluder(shape);
}
}