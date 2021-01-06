#include "SaffronPCH.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "Saffron/Graphics/Scene.h"

namespace Se
{
Scene::Scene(String name, ControllableRenderTexture *target, Camera *camera)
	: _target(target),
	_camera(camera),
	_viewportPane(Move(name), *_target)
{
}

void Scene::OnUpdate()
{
    if(_viewportPane.IsHovered())
    {
        _camera->OnUpdate();
    }
}

void Scene::OnGuiRender()
{
	_viewportPane.OnGuiRender();
}

void Scene::OnRenderTargetResize(const sf::Vector2f &size)
{
    _camera->OnUpdate();
}

void Scene::Submit(const sf::Drawable &drawable, sf::RenderStates renderStates)
{
	if ( !_screenSpaceDrawing )
	{
		renderStates.transform.combine(_camera->GetTransform());
	}
	_target->GetRenderTexture().draw(drawable, renderStates);
}

void Scene::Submit(const sf::Text &text, TextAlign align, sf::RenderStates renderStates)
{
	auto textCpy = text;
	float offset = 0.0f;
	switch ( align )
	{
	case TextAlign::Left:
		break;
	case TextAlign::Middle:
		offset = textCpy.getLocalBounds().width / 2.0f;
		break;
	case TextAlign::Right:
		offset = textCpy.getLocalBounds().width;
		break;
	}

	textCpy.setPosition(text.getPosition().x - offset, text.getPosition().y);
	Submit(textCpy, renderStates);
}

void Scene::Submit(const sf::Vector2f &position, sf::Color color, float radius)
{
	sf::CircleShape circle;
	const float adjustedRadius = radius / _camera->GetZoom();
	circle.setPosition(position - sf::Vector2f(adjustedRadius, adjustedRadius));
	circle.setFillColor(color);
	circle.setRadius(adjustedRadius);
	Submit(circle);
}

void Scene::Submit(const sf::FloatRect &rect, sf::Color fillColor, bool outlined, sf::Color outlineColor)
{
	sf::RectangleShape rectShape;
	rectShape.setPosition(rect.left, rect.top);
	rectShape.setSize(sf::Vector2f(rect.width, rect.height));
	rectShape.setFillColor(fillColor);
	if ( outlined )
	{
		rectShape.setOutlineThickness(1.0f / _camera->GetZoom());
		rectShape.setOutlineColor(outlineColor);
	}
	Submit(rectShape);
}

void Scene::Submit(const sf::Vector2f &first, const sf::Vector2f &second, sf::Color color)
{
	sf::VertexArray line(sf::PrimitiveType::Lines, 2);
	line[0].color = color;
	line[1].color = color;
	line[0].position = first;
	line[1].position = second;
	Submit(line);
}
}
