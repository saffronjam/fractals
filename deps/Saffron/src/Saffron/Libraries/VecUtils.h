#pragma once

#include <utility>
#include <cmath>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Color.hpp>

namespace Se
{
class VecUtils
{
public:
	template <typename T>
	static sf::Vector2<T> LineMiddlePoint(const sf::Vector2<T> &point1, const sf::Vector2<T> &point2);

	template <typename T>
	static sf::Vector2<T> Direction(const sf::Vector2<T> &point1, const sf::Vector2<T> &point2);

	template <typename T>
	static sf::Vector2<T> Unit(const sf::Vector2<T> &vector);

	template <typename T = float>
	static sf::Vector2<T> Null();

	template <typename T>
	static sf::Vector2<T> Perpendicular(const sf::Vector2<T> &vector);

	template <typename T>
	static sf::Vector2<T> Rotate(const sf::Vector2<T> &vector, float angle, const sf::Vector2<T> &around);

	template <typename T>
	static sf::Vector2<T> Rotate(const sf::Vector2<T> &vector, const sf::Vector2f &direction, const sf::Vector2<T> &around);

	template <typename T>
	static float Length(const sf::Vector2<T> &vector);

	template <typename T>
	static float LengthSq(const sf::Vector2<T> &vector);

	template <typename T>
	static float Distance(const sf::Vector2<T> &u, const sf::Vector2<T> &v);

	template <typename T>
	static void Normalize(sf::Vector2<T> &vector);

	template <typename T>
	static sf::Vector2<T> SetLength(const sf::Vector2<T> &vector, float length);

	template <typename T>
	static float Angle(const sf::Vector2<T> &v1, const sf::Vector2<T> &v2);

	template <typename T>
	static float Slope(sf::Vector2<T> point1, sf::Vector2<T> point2);

	template <typename T>
	static float DistanceFromLine(sf::Vector2<T> linePoint1, sf::Vector2<T> linePoint2, sf::Vector2<T> point);

	template <typename T>
	static T Dot(const sf::Vector2<T> &u, const sf::Vector2<T> &v);

	template <typename T>
	static sf::Vector2<T> MapRange(const sf::Vector2<T> &v, T a1, T b1, T a2, T b2);

	template <typename T>
	static sf::Vector3<T> MapRange(const sf::Vector3<T> &v, T a1, T b1, T a2, T b2);

	template <typename T = float>
	static sf::Vector3<T> ByColor(const sf::Color &color);

	template <typename T, typename U>
	static T ConvertTo(const U &in);

	template <typename T>
	static bool IsLeft(const sf::Vector2<T> &a, const sf::Vector2<T> &b, const sf::Vector2<T> &point);

	template <typename T>
	static sf::Vector2<T> Constrain(const sf::Vector2<T> &value, T from, T to);
};

template <typename T>
sf::Vector2<T> VecUtils::LineMiddlePoint(const sf::Vector2<T> &point1, const sf::Vector2<T> &point2)
{
	sf::Vector2<T> delta = point2 - point1;
	delta.x /= 2.0f;
	delta.y /= 2.0f;
	return point1 + delta;
}

template <typename T>
sf::Vector2<T> VecUtils::Direction(const sf::Vector2<T> &point1, const sf::Vector2<T> &point2)
{
	return VecUtils::Unit(point2 - point1);
}

template <typename T>
sf::Vector2<T> VecUtils::Unit(const sf::Vector2<T> &vector)
{
	if ( vector != VecUtils::Null<T>() )
		return vector / VecUtils::Length(vector);
	else
		return VecUtils::Null<T>();
}

template <typename T>
sf::Vector2<T> VecUtils::Null()
{
	return sf::Vector2<T>(0, 0);
}

template <typename T>
sf::Vector2<T> VecUtils::Perpendicular(const sf::Vector2<T> &vector)
{
	return sf::Vector2<T>(-vector.y, vector.x);
}

template <typename T>
sf::Vector2<T> VecUtils::Rotate(const sf::Vector2<T> &vector, float angle, const sf::Vector2<T> &around)
{
	auto anchor = vector - around;

	const float cosTheta = cos(angle);
	const float sinTheta = sin(angle);
	const float new_x = anchor.x * cosTheta - anchor.y * sinTheta;
	anchor.y = anchor.x * sinTheta + anchor.y * cosTheta;
	anchor.x = new_x;

	return anchor + around;
}

template <typename T>
sf::Vector2<T> VecUtils::Rotate(const sf::Vector2<T> &vector, const sf::Vector2f &direction, const sf::Vector2<T> &around)
{
	const sf::Vector2f right(1.0f, 0.0f);

	float angle = VecUtils::Angle(right, direction);
	if ( direction.y > 0.0f )
	{
		angle = 360.0f - angle;
	}
	return VecUtils::Rotate(vector, angle, around);
}

template <typename T>
float VecUtils::Length(const sf::Vector2<T> &vector)
{
	return sqrt(VecUtils::LengthSq(vector));
}

template <typename T>
float VecUtils::LengthSq(const sf::Vector2<T> &vector)
{
	return vector.x * vector.x + vector.y * vector.y;
}

template <typename T>
float VecUtils::Distance(const sf::Vector2<T> &u, const sf::Vector2<T> &v)
{
	return sqrt(pow(u.x - v.x, 2) + pow(u.y - v.y, 2));
}

template <typename T>
void VecUtils::Normalize(sf::Vector2<T> &vector)
{
	float length = VecUtils::Length(vector);
	if ( length != 0.0f )
	{
		vector.x /= length;
		vector.y /= length;
	}
}

template <typename T>
sf::Vector2<T> VecUtils::SetLength(const sf::Vector2<T> &vector, float length)
{
	return VecUtils::Unit(vector) * length;
}

template <typename T>
float VecUtils::Angle(const sf::Vector2<T> &v1, const sf::Vector2<T> &v2)
{
	return std::acos(VecUtils::Dot(v1, v2) / (VecUtils::Length(v1) * VecUtils::Length(v2)));
}

template <typename T>
float VecUtils::Slope(sf::Vector2<T> point1, sf::Vector2<T> point2)
{
	if ( point1.x > point2.x )
	{
		std::swap(point1, point2);
	}
	return (point2.y - point1.y) / (point2.x - point1.x);
}

template <typename T>
float VecUtils::DistanceFromLine(sf::Vector2<T> linePoint1, sf::Vector2<T> linePoint2, sf::Vector2<T> point)
{
	return abs(((linePoint2.x - linePoint1.x) * (point.y - linePoint1.y) - (linePoint2.y - linePoint1.y) * (point.x - linePoint1.x)) / VecUtils::Length(linePoint2 - linePoint1));
}

template <typename T>
T VecUtils::Dot(const sf::Vector2<T> &u, const sf::Vector2<T> &v)
{
	return u.x * v.x + u.y * v.y;
}

template <typename T>
sf::Vector2<T> VecUtils::MapRange(const sf::Vector2<T> &v, T a1, T b1, T a2, T b2)
{
	T x = (v.x - a1) * (b2 - a2) / (b1 - a1) + a2;
	T y = (v.y - a1) * (b2 - a2) / (b1 - a1) + a2;
	return sf::Vector2<T>(x, y);
}

template <typename T>
sf::Vector3<T> VecUtils::MapRange(const sf::Vector3<T> &v, T a1, T b1, T a2, T b2)
{
	T x = (v.x - a1) * (b2 - a2) / (b1 - a1) + a2;
	T y = (v.y - a1) * (b2 - a2) / (b1 - a1) + a2;
	T z = (v.z - a1) * (b2 - a2) / (b1 - a1) + a2;
	return sf::Vector3<T>(x, y, z);
}

template <typename T>
sf::Vector3<T> VecUtils::ByColor(const sf::Color &color)
{
	return sf::Vector3<T>((T)color.r, (T)color.g, (T)color.b);
}

template <typename T, typename U>
T VecUtils::ConvertTo(const U &in)
{
	return T(in.x, in.y);
}

template <typename T>
bool VecUtils::IsLeft(const sf::Vector2<T> &a, const sf::Vector2<T> &b, const sf::Vector2<T> &point)
{
	return (b.x - a.x) * (point.y - a.y) - (b.y - a.y) * (point.x - a.x) < 0.0f;
}

template <typename T>
sf::Vector2<T> VecUtils::Constrain(const sf::Vector2<T> &vector, T from, T to)
{
	if ( VecUtils::Length(vector) < from )
	{
		return VecUtils::Unit(vector) * from;
	}
	else if ( VecUtils::Length(vector) > to )
	{
		return VecUtils::Unit(vector) * to;
	}
	return vector;
}
}