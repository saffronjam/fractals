#include "SaffronPCH.h"

#include "Saffron/Lighting/PointLight.h"

namespace Se
{
PointLight::Ptr PointLight::Create(const sf::Vector2f &position, float radius, sf::Color color)
{
    Ptr ret = std::make_shared<PointLight>();
    ret->BuildBase(position, radius, color);
    return ret;
}
}
