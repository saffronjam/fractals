#pragma once

#include <Saffron.h>

namespace Se
{
enum class PaletteType
{
	Fiery,
	FieryAlt,
	UV,
	GreyScale,
	Rainbow
};

class PaletteManager : public SingleTon<PaletteManager>
{
public:
	PaletteManager();

	void OnUpdate();

	void UploadTexture();
	auto Texture() const -> const sf::Texture&;

	auto Desired() const -> PaletteType;
	auto DesiredPixelPtr() const -> const sf::Uint8*;
	auto DesiredImage() const -> const sf::Image&;
	void SetActive(PaletteType type);

public:
	static constexpr int PaletteWidth = 2048;

	EventSubscriberList<void> PaletteUpdated;

public:
	struct TransitionColor
	{
		float r;
		float g;
		float b;
		float a;
	};

	HashMap<PaletteType, Shared<sf::Image>> _palettes;

	sf::Texture _texture;
	bool _wantTextureUpload = true;

	// Animate palette change
	PaletteType _desired;
	sf::Image _currentPalette;
	Array<TransitionColor, PaletteWidth> _colorsStart;
	Array<TransitionColor, PaletteWidth> _colorsCurrent;
	float _colorTransitionTimer = 0.0f;
	float _colorTransitionDuration = 0.7f;
};
}
