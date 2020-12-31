#pragma once

#include <map>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

namespace Se
{
class Mouse
{
public:
	Mouse() = default;
	~Mouse() = default;
	Mouse(const Mouse &) = delete;
	Mouse &operator=(const Mouse &) = delete;

	// Move buttonMap into prev-buttonMap
	static void OnUpdate();
	static void OnEvent(const sf::Event &event);

	static bool IsDown(const sf::Mouse::Button &button);
	static bool IsPressed(const sf::Mouse::Button &button);
	static bool IsReleased(const sf::Mouse::Button &button);
	static bool AnyButtonDown();

	static const sf::Vector2f &GetPosition(bool normalized = false);
	static const sf::Vector2f &GetSwipe();

	static float GetVerticalScroll() { return _verticalScrollBuffer; }
	static float GetHorizontalScroll() { return _horizontalScrollBuffer; }

private:
	static void OnPress(const sf::Event::MouseButtonEvent &event);
	static void OnRelease(const sf::Event::MouseButtonEvent &event);
	static void OnMove(const sf::Event::MouseMoveEvent &event);
	static void OnEnter();
	static void OnLeave();
	static void OnScroll(const sf::Event::MouseWheelScrollEvent &event);

private:
	static std::map<sf::Mouse::Button, bool> _buttonmap;
	static std::map<sf::Mouse::Button, bool> _prevButtonmap;

	static sf::Vector2f _mousePosition;
	static sf::Vector2f _mousePositionNDC;
	static bool _inWindow;

	static sf::Vector2f _mouseSwipe;

	static float _verticalScrollBuffer;
	static float _horizontalScrollBuffer;
};
}