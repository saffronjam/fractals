#pragma once

#include "Saffron/Base.h"
#include "Saffron/Math/SaffronMath.h"
#include "Saffron/Graphics/ControllableRenderTexture.h"

namespace Se
{
class ViewportPane : public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<void> OnPostRender;
		static SignalAggregate<const sf::Vector2f &> OnWantRenderTargetResize;
	};

public:
	explicit ViewportPane(String windowTitle, const ControllableRenderTexture &target);

	void OnGuiRender(bool *open = nullptr, UUID uuid = 0);

	bool InViewport(sf::Vector2f positionNDC) const;

	sf::Vector2f GetMousePosition(bool normalized = false) const;
	sf::Vector2f GetViewportSize() const;
	Uint32 GetDockID() const { return _dockID; }

	const sf::Vector2f &GetTopLeft() const { return _topLeft; }
	const sf::Vector2f &GetBottomRight() const { return _bottomRight; }

	bool IsHovered() const { return _hovered; }
	bool IsFocused() const { return _focused; }

	void SetTarget(const ControllableRenderTexture &target) { _target = &target; }

private:
	String _windowTitle;
	const ControllableRenderTexture *_target;
	sf::Texture _fallbackTexture;
	Uint32 _dockID = 0;

	sf::Vector2f _topLeft;
	sf::Vector2f _bottomRight;
	bool _hovered;
	bool _focused;

};

}