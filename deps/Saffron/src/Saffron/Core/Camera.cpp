#include "SaffronPCH.h"

#include "Saffron/Core/Application.h"
#include "Saffron/Core/Camera.h"
#include "Saffron/Core/Global.h"
#include "Saffron/Gui/Gui.h"
#include "Saffron/Input/Keyboard.h"
#include "Saffron/Input/Mouse.h"
#include "Saffron/Lighting/LightningMgr.h"

namespace Se
{
Camera::Camera()
	: _transform(sf::Transform::Identity),
	_positionTransform(sf::Transform::Identity),
	_rotationTransform(sf::Transform::Identity),
	_zoomTransform(sf::Transform::Identity),
	_position(0.0f, 0.0f),
	_rotation(0.0f),
	_rps(0.2f),
	_zoom(1.0f, 1.0f)
{
}

void Camera::OnUpdate()
{
	const auto dt = Global::Clock::GetFrameTime();

	if ( _follow.has_value() )
	{
		SetCenter(*_follow.value());
	}
	else
	{
		if ( Mouse::IsDown(sf::Mouse::Button::Left) && Mouse::IsDown(sf::Mouse::Button::Right) )
		{
			sf::Vector2f delta = Mouse::GetSwipe();
			if ( VecUtils::LengthSq(delta) > 0.0f )
			{
				delta = _rotationTransform.getInverse().transformPoint(delta);
				delta = _zoomTransform.getInverse().transformPoint(delta);
				delta *= -1.0f;
				Move(delta);
			}
		}
	}

	Zoom((Mouse::GetVerticalScroll() / 100.0f) + 1.0f);

	float angle = 0.0f;

	if ( Keyboard::IsDown(sf::Keyboard::Q) )
	{
		angle += _rps * 360.0f * dt.asSeconds();
	}
	if ( Keyboard::IsDown(sf::Keyboard::E) )
	{
		angle -= _rps * 360.0f * dt.asSeconds();
	}
	Rotate(angle);

	if ( Keyboard::IsPressed(sf::Keyboard::R) )
	{
		ResetTransformation();
	}
}

void Camera::OnGuiRender()
{
	if ( ImGui::Begin("Camera") )
	{
		Gui::BeginPropertyGrid();
		Gui::Property("Position", _position, -10000.0f, 10000.0f, 10.0f, Gui::PropertyFlag_Drag);
		if ( Gui::Property("Zoom", _zoom.x, 0.01f, 10.0f, 0.01f, Gui::PropertyFlag_Slider) )
		{
			_zoom.y = _zoom.x;
		}
		Gui::Property("Rotation", _rotation, -180.0f, 180.0f, 1.0f, Gui::PropertyFlag_Slider);
		Gui::Property("Rotation Speed", _rps, 0.0f, 10.0f, 0.1f, Gui::PropertyFlag_Slider);

		if ( _follow.has_value() )
		{
			auto follow = *_follow.value();
			Gui::Property("Follow", follow, Gui::PropertyFlag_Drag);
		}
		else
		{
			Gui::Property("Follow", "Not set");
		}

		Gui::EndPropertyGrid();

	}
	ImGui::End();
}

void Camera::Move(const sf::Vector2f &offset)
{
	SetCenter(_position + offset);
}

void Camera::Zoom(float factor)
{
	_zoom *= factor;
	_zoomTransform.scale(factor, factor);
	UpdateTransform();
}

void Camera::Rotate(float angle)
{
	SetRotation(_rotation + angle);
}

void Camera::SetCenter(const sf::Vector2f &center)
{
	_position = center;
	_positionTransform = sf::Transform().translate(_position);
	UpdateTransform();
}

void Camera::SetZoom(float zoom)
{
	if ( zoom != 0.0f )
	{
		LightningMgr::ChangeResolution(zoom / _zoom.x);
		_zoom = sf::Vector2f(zoom, zoom);
		_zoomTransform = sf::Transform().scale(_zoom, _zoom);
		UpdateTransform();
	}
}

void Camera::SetRotation(float angle)
{
	_rotation = angle;
	_rotationTransform = sf::Transform().rotate(_rotation);
	UpdateTransform();
}

sf::Vector2f Camera::ScreenToWorld(const sf::Vector2f &point)
{
	return _transform.getInverse().transformPoint(point);
}

sf::FloatRect Camera::ScreenToWorld(const sf::FloatRect &rect)
{
	return _transform.getInverse().transformRect(rect);
}

sf::Vector2f Camera::WorldToScreen(const sf::Vector2f &point)
{
	return _transform.transformPoint(point);
}

sf::FloatRect Camera::WorldToScreen(const sf::FloatRect &rect)
{
	return _transform.transformRect(rect);
}

Pair<sf::Vector2f, sf::Vector2f> Camera::GetViewport() const
{
	const sf::FloatRect screenRect = { {0.0f, 0.0f}, _viewportSize };
	const auto TL = sf::Vector2f(screenRect.left, screenRect.top);
	const auto BR = sf::Vector2f(screenRect.left + screenRect.width, screenRect.top + screenRect.height);

	return std::make_pair(_transform.getInverse().transformPoint(TL), _transform.getInverse().transformPoint(BR));
}

sf::Vector2f Camera::GetOffset() const
{
	return _viewportSize / 2.0f;;
}

void Camera::UpdateTransform()
{
	_transform = sf::Transform::Identity;
	_transform.translate(GetOffset());
	_transform.scale(_zoom);
	_transform.rotate(_rotation);
	_transform.translate(-_position);
}

void Camera::CapZoomLevel() const
{
	GenUtils::Constrain(_zoom.x, 0.9f, 3.0f);
	GenUtils::Constrain(_zoom.y, 0.9f, 3.0f);
}

void Camera::ResetTransformation()
{
	SetCenter(sf::Vector2f(0.0f, 0.0f));
	SetRotation(0.0f);
	SetZoom(1.0f);

	_positionTransform = sf::Transform::Identity;
	_rotationTransform = sf::Transform::Identity;
	_zoomTransform = sf::Transform::Identity;
}
}
