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

class PaletteManager : public Singleton<PaletteManager>
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

	SubscriberList<void> PaletteUpdated;

public:
	struct TransitionColor
	{
		float r;
		float g;
		float b;
		float a;
	};

	std::unordered_map<PaletteType, std::shared_ptr<sf::Image>> _palettes;

	sf::Texture _texture;
	bool _wantTextureUpload = true;

	// Animate palette change
	PaletteType _desired;
	sf::Image _currentPalette;
	std::array<TransitionColor, PaletteWidth> _colorsStart;
	std::array<TransitionColor, PaletteWidth> _colorsCurrent;
	float _colorTransitionTimer = 0.0f;
	float _colorTransitionDuration = 0.7f;
};
}
