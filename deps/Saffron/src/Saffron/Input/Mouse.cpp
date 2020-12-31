#include "SaffronPCH.h"

#include <imgui.h>

#include "Saffron/Input/Mouse.h"

namespace Se
{
std::map<sf::Mouse::Button, bool> Mouse::_buttonmap;
std::map<sf::Mouse::Button, bool> Mouse::_prevButtonmap;
sf::Vector2f Mouse::_mousePosition = sf::Vector2f(0.0f, 0.0f);
sf::Vector2f Mouse::_mousePositionNDC = sf::Vector2f(0.0f, 0.0f);
bool Mouse::_inWindow = true;
sf::Vector2f Mouse::_mouseSwipe = sf::Vector2f(0.0f, 0.0f);
float Mouse::_verticalScrollBuffer = 0.0f;
float Mouse::_horizontalScrollBuffer = 0.0f;

void Mouse::OnUpdate()
{
	for ( auto &[key, state] : _buttonmap )
	{
		_prevButtonmap[key] = state;
	}

	_horizontalScrollBuffer = 0.0f;
	_verticalScrollBuffer = 0.0f;
	_mouseSwipe = { 0.0f, 0.0f };
}

void Mouse::OnEvent(const sf::Event &event)
{
	switch ( event.type )
	{
	case sf::Event::EventType::MouseButtonPressed:
		OnPress(event.mouseButton);
		break;
	case sf::Event::EventType::MouseButtonReleased:
		OnRelease(event.mouseButton);
		break;
	case sf::Event::EventType::MouseMoved:
		OnMove(event.mouseMove);
		break;
	case sf::Event::EventType::MouseEntered:
		OnEnter();
		break;
	case sf::Event::EventType::MouseLeft:
		OnLeave();
		break;
	case sf::Event::EventType::MouseWheelScrolled:
		OnScroll(event.mouseWheelScroll);
		break;
	default:
		break;
	}
}

bool Mouse::IsDown(const sf::Mouse::Button &button)
{
	return _buttonmap[button];
}

bool Mouse::IsPressed(const sf::Mouse::Button &button)
{
	return _buttonmap[button] && !_prevButtonmap[button];
}

bool Mouse::IsReleased(const sf::Mouse::Button &button)
{
	return !_buttonmap[button] && _prevButtonmap[button];
}

bool Mouse::AnyButtonDown()
{
	for ( auto &[button, state] : _buttonmap )
		if ( state )
			return true;
	return false;
}

const sf::Vector2f &Mouse::GetPosition(bool normalized)
{
	if ( !normalized )
	{
		return _mousePosition;
	}

	const ImVec2 imMousePosition = ImGui::GetMousePos();
	_mousePosition.x = imMousePosition.x;
	_mousePosition.y = imMousePosition.y;
	return _mousePosition;
}

const sf::Vector2f &Mouse::GetSwipe()
{
	return _mouseSwipe;
}

void Mouse::OnPress(const sf::Event::MouseButtonEvent &event)
{
	_buttonmap[event.button] = true;
}

void Mouse::OnRelease(const sf::Event::MouseButtonEvent &event)
{
	_buttonmap[event.button] = false;
}

void Mouse::OnMove(const sf::Event::MouseMoveEvent &event)
{
	if ( !_inWindow && AnyButtonDown() || _inWindow )
	{
		const auto oldPosition = _mousePosition;
		_mousePosition = sf::Vector2f(event.x, event.y);
		_mouseSwipe = _mousePosition - oldPosition;
	}
}

void Mouse::OnEnter()
{
	_inWindow = true;
}

void Mouse::OnLeave()
{
	_inWindow = false;
}

void Mouse::OnScroll(const sf::Event::MouseWheelScrollEvent &event)
{
	if ( event.wheel == sf::Mouse::Wheel::HorizontalWheel )
	{
		_horizontalScrollBuffer += event.delta;
	}
	else if ( event.wheel == sf::Mouse::Wheel::VerticalWheel )
	{
		_verticalScrollBuffer += event.delta;
	}
}
}
