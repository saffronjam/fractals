#include "SaffronPCH.h"

#include "Saffron/Lighting/Light.h"

namespace Se
{
void Light::BuildBase(const sf::Vector2f &position, float radius, sf::Color color, const sf::Vector2f &direction,
                      float theta)
{
    _position = position;
    _radius = radius;
    _color = color;
    _staticOccluderMap.create(GetBoundingBox().width, GetBoundingBox().height);
    _staticOccluderMap.clear(sf::Color(0, 0, 0, 0));
}

void Light::AddStaticOccluder(const sf::Drawable &drawable, const sf::RenderStates &renderStates)
{
    _staticOccluderMap.draw(drawable, renderStates);
    _staticOccluderMap.display();
}
}