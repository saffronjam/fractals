#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include "Saffron/Math/Vector4.h"

namespace Se
{
class Math
{
public:
	static constexpr float PI = 3.141592653589793238462643383279f;
	static constexpr double PI_D = static_cast<double>(PI);

	static constexpr float E = 2.71828182845904523536f;
	static constexpr double E_D = static_cast<double>(E);
};

}
