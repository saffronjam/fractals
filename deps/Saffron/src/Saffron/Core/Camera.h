#pragma once

#include <optional>

#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>

#include "Saffron/Base.h"
#include "Saffron/Core/Window.h"
#include "Saffron/Libraries/GenUtils.h"

namespace Se
{
class Camera
{
public:
	Camera();

	void OnUpdate();
	void OnGuiRender();

	void Move(const sf::Vector2f &offset);
	void Zoom(float factor);
	void Rotate(float angle);

	void SetCenter(const sf::Vector2f &center);
	void SetZoom(float zoom);
	void SetRotation(float angle);

	void Follow(const sf::Vector2f *follow) { _follow = follow; }
	void Unfollow() { _follow.reset(); }

	///Translate a point to world space
	///@param point: point to be translated from screen to world space.
	sf::Vector2f ScreenToWorld(const sf::Vector2f &point);
	///Translate a rect to world space
	///@param rect: rect to be translated from screen to world space.
	sf::FloatRect ScreenToWorld(const sf::FloatRect &rect);
	///Translate a point to world space
	///@param point: point to be translated from screen to screen space.
	sf::Vector2f WorldToScreen(const sf::Vector2f &point);
	///Translate a rect to world space
	///@param rect: rect to be translated from world to screen space.
	sf::FloatRect WorldToScreen(const sf::FloatRect &rect);

	const sf::Transform &GetTransform() const { return _transform; }
	Pair<sf::Vector2f, sf::Vector2f> GetViewport() const;
	sf::Vector2f GetOffset() const;
	float GetZoom() const { return _zoom.x; }

	void SetViewportSize(const sf::Vector2f &viewportSize) { _viewportSize = viewportSize; }
	void SetRotationSpeed(float rps) { _rps = rps; }

private:
	void UpdateTransform();
	void CapZoomLevel() const;
	void ResetTransformation();

private:
	sf::Transform _transform;
	sf::Transform _positionTransform;
	sf::Transform _rotationTransform;
	sf::Transform _zoomTransform;

	sf::Vector2f _position;
	float _rotation;
	float _rps; // Rotations per second
	sf::Vector2f _zoom;
	sf::Vector2f _viewportSize;

	std::optional<const sf::Vector2f *> _follow;
};
}