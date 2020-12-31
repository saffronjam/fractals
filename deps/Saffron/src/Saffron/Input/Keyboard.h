#pragma once

#include <map>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace Se
{
class Keyboard
{
public:
	Keyboard() = default;
	~Keyboard() = default;
	Keyboard(const Keyboard &) = delete;
	Keyboard &operator=(const Keyboard &) = delete;

	static void OnUpdate();
	static void OnEvent(const sf::Event &event);

	static bool IsDown(const sf::Keyboard::Key &key);
	static bool IsPressed(const sf::Keyboard::Key &key);
	static bool IsReleased(const sf::Keyboard::Key &key);
	static bool IsAnyDown();
	static std::string GetTextInput() { return _textInputBuffer; }

private:

	static void OnPress(const sf::Event::KeyEvent &event);
	static void OnRelease(const sf::Event::KeyEvent &event);
	static void OnTextInput(unsigned char character);

private:
	static std::map<sf::Keyboard::Key, bool> _keymap;
	static std::map<sf::Keyboard::Key, bool> _prevKeymap;
	static std::string _textInputBuffer;
};
}