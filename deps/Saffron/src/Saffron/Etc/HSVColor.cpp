#include "SaffronPCH.h"

#include "Saffron/Etc/HSVColor.h"

namespace Se
{
HSVColor::HSVColor()
	: HSVColor(0, 0.0f, 0.0f)
{
}

HSVColor::HSVColor(int hue, float saturation, float value)
	: sf::Color(GenUtils::HSVtoRGB(hue, saturation, value)),
	_hue(hue),
	_saturation(saturation), _value(value)
{
}

HSVColor::HSVColor(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b, sf::Uint8 a)
	: HSVColor(sf::Color(r, g, b, a))
{
}

HSVColor::HSVColor(const sf::Color &color)
	: HSVColor(GenUtils::RGBtoHSV(color))
{
}

HSVColor::HSVColor(const HSVColor &hsvColor)
	: HSVColor(hsvColor._hue, hsvColor._saturation, hsvColor._value)
{
}

const sf::Color &HSVColor::GetRGBA() const
{
	return *this;
}

int HSVColor::GetHue() const
{
	return _hue;
}

float HSVColor::GetSaturation() const
{
	return _saturation;
}

float HSVColor::GetValue() const
{
	return _value;
}

void HSVColor::SetHue(int hue)
{
	_hue = hue;
	GenerateRGB();
}

void HSVColor::SetSaturation(float saturation)
{
	_saturation = saturation;
	GenerateRGB();
}

void HSVColor::SetValue(float value)
{
	_value = value;
	GenerateRGB();
}

void HSVColor::GenerateRGB()
{
	const auto color = GenUtils::HSVtoRGB(_hue, _saturation, _value);
	r = color.r;
	g = color.g;
	b = color.b;
}
}
