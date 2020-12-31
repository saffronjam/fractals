#include "SaffronPCH.h"

#include "Saffron/Graphics/ControllableRenderTexture.h"

namespace Se
{
ControllableRenderTexture::ControllableRenderTexture(int width, int height, sf::ContextSettings contextSettings)
{
	_renderTexture.create(width, height, contextSettings);
}
}
