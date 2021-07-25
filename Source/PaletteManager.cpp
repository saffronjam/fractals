#include "PaletteManager.h"

#include "glad/glad.h"

namespace Se
{
PaletteManager::PaletteManager() :
	SingleTon(this),
	_desired(PaletteType::Fiery)
{
	// Create and initial upload
	_texture.create(PaletteWidth, 1);

	_palettes.emplace(PaletteType::Fiery, ImageStore::Get("Pals/fieryRec.png"));
	_palettes.emplace(PaletteType::FieryAlt, ImageStore::Get("Pals/fiery.png"));
	_palettes.emplace(PaletteType::UV, ImageStore::Get("Pals/uvRec.png"));
	_palettes.emplace(PaletteType::GreyScale, ImageStore::Get("Pals/greyscaleRec.png"));
	_palettes.emplace(PaletteType::Rainbow, ImageStore::Get("Pals/rainbowRec.png"));

	// Assert each palette is at least PaletteWidth wide
	for (const auto& image : _palettes | std::views::values)
	{
		Debug::Assert(image->getSize().x >= PaletteWidth && image->getSize().y >= 1);
	}

	_currentPalette.create(PaletteWidth, 1, _palettes.at(_desired)->getPixelsPtr());

	for (int i = 0; i < PaletteWidth; i++)
	{
		const auto pix = _currentPalette.getPixel(i, 0);
		_colorsStart[i] = {
			static_cast<float>(pix.r) / 255.0f, static_cast<float>(pix.g) / 255.0f, static_cast<float>(pix.b) / 255.0f,
			static_cast<float>(pix.a) / 255.0f
		};
	}
	_colorsCurrent = _colorsStart;
}

void PaletteManager::OnUpdate()
{
	if (_wantTextureUpload)
	{
		glBindTexture(GL_TEXTURE_2D, _texture.getNativeHandle());
		const auto size = _texture.getSize();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			_currentPalette.getPixelsPtr());
		glBindTexture(GL_TEXTURE_2D, 0);
		_wantTextureUpload = false;
	}

	if (_colorTransitionTimer <= _colorTransitionDuration)
	{
		const float delta = (std::sin((_colorTransitionTimer / _colorTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) /
			2.0f;
		for (int x = 0; x < PaletteWidth; x++)
		{
			const auto pix = _palettes.at(_desired)->getPixel(x, 0);
			const TransitionColor goalColor = {
				static_cast<float>(pix.r) / 255.0f, static_cast<float>(pix.g) / 255.0f,
				static_cast<float>(pix.b) / 255.0f, static_cast<float>(pix.a) / 255.0f
			};
			const auto& startColor = _colorsStart[x];
			auto& currentColor = _colorsCurrent[x];
			currentColor.r = startColor.r + delta * (goalColor.r - startColor.r);
			currentColor.g = startColor.g + delta * (goalColor.g - startColor.g);
			currentColor.b = startColor.b + delta * (goalColor.b - startColor.b);
			_currentPalette.setPixel(x, 0, {
				                         static_cast<sf::Uint8>(currentColor.r * 255.0f),
				                         static_cast<sf::Uint8>(currentColor.g * 255.0f),
				                         static_cast<sf::Uint8>(currentColor.b * 255.0f),
				                         static_cast<sf::Uint8>(currentColor.a * 255.0f)
			                         });
		}

		PaletteUpdated.Invoke();
		_colorTransitionTimer += Global::Clock::FrameTime().asSeconds();
	}
}

void PaletteManager::UploadTexture()
{
	_wantTextureUpload = true;
}

auto PaletteManager::Texture() const -> const sf::Texture&
{
	return _texture;
}

auto PaletteManager::Desired() const -> PaletteType
{
	return _desired;
}

auto PaletteManager::DesiredPixelPtr() const -> const sf::Uint8*
{
	return _currentPalette.getPixelsPtr();
}


auto PaletteManager::DesiredImage() const -> const sf::Image&
{
	Debug::Assert(_palettes.contains(_desired));
	return *_palettes.at(_desired);
}

void PaletteManager::SetActive(PaletteType type)
{
	_desired = type;
	_colorTransitionTimer = 0.0f;
	_colorsStart = _colorsCurrent;
}
}
