#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Saffron/Base.h"
#include "Saffron/Core/Camera.h"
#include "Saffron/Graphics/ControllableRenderTexture.h"
#include "Saffron/Gui/ViewportPane.h"

namespace Se
{
class Scene
{
public:
	Scene(String name, ControllableRenderTexture *target, Camera *camera);

	void OnUpdate();
	void OnGuiRender();
	void OnRenderTargetResize(const sf::Vector2f& size);

	void ActivateScreenSpaceDrawing() { _screenSpaceDrawing = true; }
	void DeactivateScreenSpaceDrawing() { _screenSpaceDrawing = false; }

	void Submit(const sf::Drawable &drawable, sf::RenderStates renderStates = sf::RenderStates());
	void Submit(const sf::Text &text, TextAlign align, sf::RenderStates renderStates = sf::RenderStates());
	void Submit(const sf::Vector2f &position, sf::Color color, float radius);
	void Submit(const sf::FloatRect &rect, sf::Color fillColor, bool outlined, sf::Color outlineColor);
	void Submit(const sf::Vector2f &first, const sf::Vector2f &second, sf::Color color);

	Camera &GetCamera() { return *_camera; }
	const Camera &GetCamera() const { return *_camera; }

	ViewportPane &GetViewportPane() { return _viewportPane; }
	const ViewportPane &GetViewportPane() const { return _viewportPane; }

private:
	ControllableRenderTexture *_target;
	Camera *_camera;
	ViewportPane _viewportPane;

	bool _screenSpaceDrawing = false;

};
}
