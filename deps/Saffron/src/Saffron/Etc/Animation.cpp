#include "SaffronPCH.h"

#include "Saffron/Etc/Animation.h"

namespace Se
{
Animation::Animation()
	: _texture(NULL)
{
}

void Animation::addFrame(sf::IntRect rect)
{
	_frames.push_back(rect);
}

void Animation::setSpriteSheet(const sf::Texture &texture)
{
	_texture = &texture;
}

const sf::Texture *Animation::getSpriteSheet() const
{
	return _texture;
}

std::size_t Animation::getSize() const
{
	return _frames.size();
}

const sf::IntRect &Animation::getFrame(std::size_t n) const
{
	return _frames[n];
}
}