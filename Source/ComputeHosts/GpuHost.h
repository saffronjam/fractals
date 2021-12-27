#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include "glad/glad.h"

#include "Host.h"
#include "PaletteManager.h"

namespace Se
{
template<class ShaderClass>
class GpuHost : public Host
{
public:
	GpuHost(std::string name, int simWidth, int simHeight);

	void OnRender(Scene& scene) override;

protected:
	void RenderImage() override;
	void Resize(int width, int height) override;
	
	virtual auto TextureHandle() const -> uint = 0;

public:
	SubscriberList<ShaderClass&> RequestUniformUpdate;

protected:
	std::shared_ptr<sf::Shader> _painterPS;
	sf::RenderTexture _target;
};


template<class ShaderClass>
GpuHost<ShaderClass>::GpuHost(std::string name, int simWidth, int simHeight) :
	Host(std::move(name), simWidth, simHeight),
	_painterPS(ShaderStore::Get("painter.frag", sf::Shader::Type::Fragment))
{
	_target.create(simWidth, simHeight);
	glBindTexture(GL_TEXTURE_2D, _target.getTexture().getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, simWidth, simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

template <class ShaderClass>
void GpuHost<ShaderClass>::OnRender(Scene& scene)
{
	scene.ActivateScreenSpaceDrawing();
	scene.Submit(sf::Sprite(_target.getTexture()));	
	scene.DeactivateScreenSpaceDrawing();
}

template<class ShaderClass>
void GpuHost<ShaderClass>::RenderImage()
{
	const auto& palTex = PaletteManager::Instance().Texture();

	glBindImageTexture(0, TextureHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, palTex.getNativeHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	SetUniform(_painterPS->getNativeHandle(), "maxPixelValue", static_cast<float>(ComputeIterations()));
	SetUniform(_painterPS->getNativeHandle(), "paletteWidth", PaletteManager::PaletteWidth);
	
	sf::RectangleShape simRectShape(sf::Vector2f(SimWidth(), SimHeight()));
	simRectShape.setTexture(&palTex);
	_target.draw(simRectShape, { _painterPS.get() });
	
}

template <class ShaderClass>
void GpuHost<ShaderClass>::Resize(int width, int height)
{
	_target.create(width, height);
}
}
