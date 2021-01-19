#pragma once

#include <cmath>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Saffron/Core/Assert.h"
#include "Saffron/Core/TypeDefs.h"
#include "Saffron/Libraries/VecUtils.h"

namespace Se
{
#define GET_VARIABLE_NAME(Variable) (#Variable)

template <typename T = float>
static constexpr T PI = static_cast<T>(3.141592653589793238462643383279);

template <typename T = float>
static constexpr T E = static_cast<T>(2.71828182845904523536);

class GenUtils
{
public:
	template <typename T, typename U>
	static sf::Vector2<T> ConvertTo(const sf::Vector2<U> &vec);

	template <typename T, typename U>
	static sf::Rect<T> ConvertTo(const sf::Rect<U> &rect);

	template <typename T>
	static sf::Vector2<T> Mid(sf::Rect<T> rect);

	static sf::Vector2f Mid(const sf::ConvexShape &polygon);

	static sf::Vector2f Mid(const ArrayList<sf::Vector2f> &polygonPoints);

	template <typename T>
	static sf::Vector2<T> MapPoint(const sf::Vector2<T> &point, sf::Rect<T> from, sf::Rect<T> to);

	template <typename T>
	static T Map(const T &value, T lowerFrom, T upperFrom, T lowerTo, T upperTo);

	template <typename T>
	static T Map(const T &value, std::pair<T, T> from, std::pair<T, T> to);

	template <typename T>
	static T Constrain(const T &value, T from, T to);

	template <typename T>
	static void Clamp(T &value, T from, T to);

	template <typename T>
	static T Clamped(const T &value, T from, T to);

	static void Rotate(sf::Transformable &transformable, const sf::Vector2f &direction);

	template <typename T>
	static T ToDegress(const T &radians);

	template <typename T>
	static T ToRadians(const T &degress);

	template <typename T>
	static sf::Color ValueToSpectrum(T value, T maxValue);

	static sf::ConvexShape CreateConvexShape(const ArrayList<sf::Vector2f> &points);

	template <typename T>
	static ArrayList<sf::Vector2<T>> WrapPoints(const ArrayList<sf::Vector2<T>> &points);

	/// @hue: 0-360°
	/// @saturation: 0.0 - 1.0
	/// @value: 0.0 - 1.0
	static sf::Color HSVtoRGB(int hue, float saturation, float value);
	static sf::Color HSVtoRGB(const class HSVColor &hsvColor);

	static class HSVColor RGBtoHSV(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b);
	static class HSVColor RGBtoHSV(const sf::Color &color);

private:
	template <typename T>
	static void ClearPointsRecursively(const std::pair<sf::Vector2<T>, sf::Vector2<T>> &line,
									   const ArrayList<sf::Vector2<T>> &points,
									   ArrayList<sf::Vector2<T>> &finalPoints);
};

template <typename T, typename U>
sf::Vector2<T> GenUtils::ConvertTo(const sf::Vector2<U> &vec)
{
	return sf::Vector2<T>((T)vec.x, (T)vec.y);
}

template <typename T, typename U>
sf::Rect<T> GenUtils::ConvertTo(const sf::Rect<U> &rect)
{
	return sf::Rect<T>((T)rect.left, (T)rect.top, (T)rect.width, (T)rect.height);
}

template <typename T>
sf::Vector2<T> GenUtils::Mid(sf::Rect<T> rect)
{
	return sf::Vector2<T>(rect.left + rect.width / (T)2, rect.top + rect.height / (T)2);
}

template <typename T>
sf::Vector2<T> GenUtils::MapPoint(const sf::Vector2<T> &point, sf::Rect<T> from, sf::Rect<T> to)
{
	sf::Vector2<T> _point = point;
	float x_diff = _point.x - from.left;
	float y_diff = _point.y - from.top;

	float x_percent_diff = x_diff / from.width;
	float y_percent_diff = y_diff / from.height;

	_point.x = to.left + to.width * x_percent_diff;
	_point.y = to.top + to.height * y_percent_diff;

	return _point;
}

template <typename T>
T GenUtils::Map(const T &value, T lowerFrom, T upperFrom, T lowerTo, T upperTo)
{
	if ( upperFrom < lowerFrom )
	{
		std::swap(lowerFrom, upperFrom);
	}
	if ( upperTo < lowerTo )
	{
		std::swap(lowerTo, upperTo);
	}

	float diffFrom = upperFrom - lowerFrom;
	const float diffTo = upperTo - lowerTo;

	const float diffFromPercent = (value - lowerFrom) / diffFrom;

	return lowerTo + diffTo * diffFromPercent;
}

template <typename T>
T GenUtils::Map(const T &value, std::pair<T, T> from, std::pair<T, T> to)
{
	return GenUtils::Map(value, from.first, from.second, to.first, to.second);
}

template <typename T>
T GenUtils::Constrain(const T &value, T from, T to)
{
	if ( value < from )
		return from;
	if ( value > to )
		return to;
	return value;
}

template <typename T>
void GenUtils::Clamp(T &value, T from, T to)
{
	value = Clamped(value, from, to);
}

template <typename T>
T GenUtils::Clamped(const T &value, T from, T to)
{
	return std::clamp(value, from, to);
}

template <typename T>
T GenUtils::ToDegress(const T &radians)
{
	return (radians * (T)180) / (T)PI<>;
}

template <typename T>
T GenUtils::ToRadians(const T &degress)
{
	return (T)(degress * PI<>) / (T)180;
}

template <typename T>
sf::Color GenUtils::ValueToSpectrum(T value, T maxValue)
{
	const double a = ((double)value / (double)maxValue) / 0.2f;
	const sf::Uint8 X = std::floor(a);
	const sf::Uint8 Y = std::floor(255 * (a - X));
	switch ( X )
	{
	case 0:
		return sf::Color(255, Y, 0);
	case 1:
		return sf::Color(255 - Y, 255, 0);
	case 2:
		return sf::Color(0, 255, Y);
	case 3:
		return sf::Color(0, 255 - Y, 255);
	case 4:
		return sf::Color(Y, 0, 255);
	case 5:
		return sf::Color(255, 0, 255);
	default:
		return sf::Color::White;
	}
};

template <typename T>
ArrayList<sf::Vector2<T>> GenUtils::WrapPoints(const ArrayList<sf::Vector2<T>> &points)
{
	ArrayList<sf::Vector2f> finalPoints;

	ArrayList<sf::Vector2f> topPoints;
	ArrayList<sf::Vector2f> bottomPoints;
	std::pair<sf::Vector2f, sf::Vector2f> startLine;

	sf::Vector2f biggestX = {
		static_cast<T>(-std::numeric_limits<T>::infinity()), -std::numeric_limits<T>::infinity()
	};
	sf::Vector2f smallestX = {
		static_cast<T>(std::numeric_limits<T>::infinity()), std::numeric_limits<T>::infinity()
	};
	for ( auto &point : points )
	{
		if ( point.x > biggestX.x )
		{
			biggestX = point;
		}
		if ( point.x < smallestX.x )
		{
			smallestX = point;
		}
	}
	startLine = std::make_pair(smallestX, biggestX);

	//Inital removal of center-points
	for ( auto &point : points )
	{
		if ( !VecUtils::IsLeft(startLine.first, startLine.second, point) )
		{
			topPoints.emplace_back(point);
		}
		else
		{
			bottomPoints.emplace_back(point);
		}
	}

	ClearPointsRecursively(startLine, topPoints, finalPoints);
	ClearPointsRecursively(std::make_pair(startLine.second, startLine.first), bottomPoints, finalPoints);

	return finalPoints;
}

template <typename T>
void GenUtils::ClearPointsRecursively(const std::pair<sf::Vector2<T>, sf::Vector2<T>> &line,
									  const ArrayList<sf::Vector2<T>> &points,
									  ArrayList<sf::Vector2<T>> &finalPoints)
{
	//Find the point which is the furthest away
	float biggestDistance = 0.0f;
	int biggestIndex = -1;
	for ( size_t i = 0; i < points.size(); i++ )
	{
		const float currentCheck = VecUtils::DistanceFromLine(line.first, line.second, points[i]);
		if ( currentCheck > biggestDistance )
		{
			biggestDistance = currentCheck;
			biggestIndex = i;
		}
	}

	auto lineCpy = line;
	//Continues only if it can find a new point
	if ( biggestIndex != -1 )
	{
		sf::Vector2f furthest = points[biggestIndex];
		std::pair<sf::Vector2f, sf::Vector2f> newLine = { lineCpy.first, furthest };
		lineCpy.first = furthest;

		ArrayList<sf::Vector2f> consideredPoints1;
		ArrayList<sf::Vector2f> consideredPoints2;
		for ( auto &point : points )
		{
			if ( !VecUtils::IsLeft(newLine.first, newLine.second, point) )
			{
				consideredPoints1.push_back(point);
			}
			else if ( !VecUtils::IsLeft(lineCpy.first, lineCpy.second, point) )
			{
				consideredPoints2.push_back(point);
			}
		}

		ClearPointsRecursively(newLine, consideredPoints1, finalPoints);
		ClearPointsRecursively(lineCpy, consideredPoints2, finalPoints);
	}
	else
	{
		finalPoints.push_back(lineCpy.first);
		finalPoints.push_back(lineCpy.second);
	}
}
}
