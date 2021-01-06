#include "SaffronPCH.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Event.hpp>

#include "Saffron/Core/Window.h"
#include "Saffron/Lighting/LightningMgr.h"

namespace Se
{
Window::Window(const std::string &title, int width, int height)
	: _videomode(width, height),
	_style(sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close),
	_nativeWindow(sf::RenderWindow(_videomode, title, _style,
								   sf::ContextSettings(0u, 0u, 0u, 1u, 4u, 0u)))

{
	const auto contexSettings = sf::ContextSettings();

	_nativeWindow.setKeyRepeatEnabled(false);
	_nativeWindow.resetGLStates();
	SetTitle(title);
	PositionCenter();
}

void Window::Draw(const sf::Drawable &drawable, sf::RenderStates renderStates)
{
	Render(drawable, renderStates);
}

void Window::DrawText(const sf::Text &text, TextAlign align, sf::RenderStates renderStates)
{
	auto textCpy = text;
	sf::Vector2f offset(0.0f, 0.0f);
	switch ( align )
	{
	case TextAlign::Left:
		break;
	case TextAlign::Middle:
		offset.x = textCpy.getLocalBounds().width / 2.0f;
		break;
	case TextAlign::Right:
		offset.x = textCpy.getLocalBounds().width;
		break;
	}

	textCpy.setPosition(text.getPosition() + offset);
	Render(textCpy, renderStates);
}

void Window::DrawPoint(const sf::Vector2f &position, sf::Color color, float radius)
{
	sf::CircleShape circle(radius);
	circle.setPosition(position - sf::Vector2f(radius, radius));
	circle.setFillColor(color);
	Draw(circle);
}

void Window::DrawRect(const sf::FloatRect &rect, sf::Color fillColor, bool outlined, sf::Color outlineColor)
{
	sf::RectangleShape rectShape;
	rectShape.setPosition(rect.left, rect.top);
	rectShape.setSize(sf::Vector2f(rect.width, rect.height));
	rectShape.setFillColor(fillColor);
	if ( outlined )
	{
		rectShape.setOutlineThickness(1);
		rectShape.setOutlineColor(outlineColor);
	}
	Draw(rectShape);
}

void Window::DrawLine(const sf::Vector2f &first, const sf::Vector2f &second, sf::Color color)
{
	sf::VertexArray line(sf::PrimitiveType::Lines, 2);
	line[0].color = color;
	line[1].color = color;
	line[0].position = first;
	line[1].position = second;
	Draw(line);
}

void Window::Clear()
{
	_nativeWindow.clear();
}

void Window::Display()
{
	_nativeWindow.display();
}

void Window::HandleBufferedEvents()
{
	sf::Event event;
	if ( _eventCallback.has_value() )
	{
		while ( _nativeWindow.pollEvent(event) )
		{
			_eventCallback.value()(event);
		}
	}
	else
	{
		while ( _nativeWindow.pollEvent(event) )
		{
		}
	}
}

void Window::SetEventCallback(Function<void(const sf::Event &)> fn)
{
	_eventCallback = Move(fn);
}

void Window::PositionCenter()
{
	const sf::VideoMode max = sf::VideoMode::getDesktopMode();
	const sf::Vector2i halfSize = sf::Vector2i(_nativeWindow.getSize().x / 2u, _nativeWindow.getSize().y / 2u);
	_nativeWindow.setPosition(sf::Vector2i(max.width, max.height) / 2 - halfSize);
}

sf::RenderWindow &Window::GetNativeWindow()
{
	return _nativeWindow;
}

const sf::RenderWindow &Window::GetNativeWindow() const
{
	return _nativeWindow;
}

sf::Vector2i Window::GetPosition()
{
	return _nativeWindow.getPosition();
}

sf::Vector2u Window::GetSize()
{
	return _nativeWindow.getSize();
}

int Window::GetWidth()
{
	return GetSize().x;
}

int Window::GetHeight()
{
	return GetSize().y;
}

const std::string &Window::GetTitle()
{
	return _title;
}

sf::IntRect Window::GetScreenRect()
{
	return sf::IntRect(0, 0, GetWidth(), GetHeight());
}

void Window::SetPosition(const sf::Vector2i &pos)
{
	_nativeWindow.setPosition(pos);
}

void Window::SetSize(const sf::Vector2u &size)
{
	_nativeWindow.setSize(size);
}

void Window::SetTitle(const std::string &title)
{
	_nativeWindow.setTitle(title);
}

void Window::SetIcon(const std::string &icon)
{
	sf::Image image;
	image.loadFromFile(icon);
	_nativeWindow.setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
}

void Window::SetFullscreen(bool toggle)
{
	if ( toggle && !_fullscreen )
	{
		_fullscreen = true;
		_videomode.width = GetSize().x;
		_videomode.height = GetSize().y;
		_nonFullscreenPosition = GetPosition();
		_nativeWindow.create(sf::VideoMode::getFullscreenModes()[0], GetTitle(), sf::Style::Fullscreen);
	}
	else if ( !toggle && _fullscreen )
	{
		_fullscreen = false;
		_nativeWindow.create(_videomode, GetTitle(), _style);
		SetPosition(_nonFullscreenPosition);
	}
}

void Window::SetVSync(bool toggle)
{
	_nativeWindow.setVerticalSyncEnabled(toggle);
}

void Window::Render(const sf::Drawable &drawable, sf::RenderStates renderStates)
{
	_nativeWindow.draw(drawable, renderStates);
}

Window::Exception::Exception(int line, const char *file, const char *errorString)
	: IException(line, file),
	errorString(errorString)
{
}

const char *Window::Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << "[Type] " << GetType() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char *Window::Exception::GetType() const
{
	return "V-2DFramework Window Exception";
}

const char *Window::Exception::GetErrorString() const
{
	return errorString.c_str();
}
}
