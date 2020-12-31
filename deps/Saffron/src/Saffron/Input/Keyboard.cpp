#include "SaffronPCH.h"

#include "Saffron/Input/Keyboard.h"

namespace Se
{
std::map<sf::Keyboard::Key, bool> Keyboard::_keymap;
std::map<sf::Keyboard::Key, bool> Keyboard::_prevKeymap;
std::string Keyboard::_textInputBuffer;

void Keyboard::OnUpdate()
{
	for ( auto &[key, state] : _keymap )
		_prevKeymap[key] = state;
	_textInputBuffer.clear();
}


void Keyboard::OnEvent(const sf::Event &event)
{
	switch ( event.type )
	{
	case sf::Event::EventType::KeyPressed:
		OnPress(event.key);
		break;
	case sf::Event::EventType::KeyReleased:
		OnRelease(event.key);
		break;
	case sf::Event::EventType::TextEntered:
		OnTextInput(event.text.unicode);
		break;
	default:
		break;
	}
}

bool Keyboard::IsDown(const sf::Keyboard::Key &key)
{
	return _keymap[key];
}

bool Keyboard::IsPressed(const sf::Keyboard::Key &key)
{
	return _keymap[key] && !_prevKeymap[key];
}

bool Keyboard::IsReleased(const sf::Keyboard::Key &key)
{
	return !_keymap[key] && _prevKeymap[key];
}

bool Keyboard::IsAnyDown()
{
	for ( auto &[key, state] : _keymap )
	{
		if ( _keymap[key] )
			return true;
	}
	return false;
}

void Keyboard::OnPress(const sf::Event::KeyEvent &event)
{
	_keymap[event.code] = true;
}

void Keyboard::OnRelease(const sf::Event::KeyEvent &event)
{
	_keymap[event.code] = false;
}

void Keyboard::OnTextInput(unsigned char character)
{
	_textInputBuffer += character;
}
}
