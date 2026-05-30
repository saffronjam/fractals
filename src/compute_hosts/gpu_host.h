#pragma once

#include <memory>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include "glad/glad.h"

#include "host.h"
#include "palette_manager.h"

namespace fractals
{
using namespace saffron;
template<class ShaderClass>
class GpuHost : public Host
{
public:
	GpuHost(HostType type, std::string name, int simWidth, int simHeight);

	void OnRender(Scene& scene) override;

protected:
	void RenderImage() override;
	void Resize(int width, int height) override;
	auto TryResizeTarget(int width, int height) -> Status;
	
	virtual auto TextureHandle() const -> uint = 0;

public:
	SubscriberList<ShaderClass&> RequestUniformUpdate;

protected:
	std::shared_ptr<sf::Shader> _painterPS;
	sf::RenderTexture _target;
};


template<class ShaderClass>
GpuHost<ShaderClass>::GpuHost(HostType type, std::string name, int simWidth, int simHeight) :
	Host(type, std::move(name), simWidth, simHeight)
{
	const auto result = TryResizeTarget(simWidth, simHeight);
	if (!result)
	{
		Log::CoreError(result.error().message);
		return;
	}
	glBindTexture(GL_TEXTURE_2D, _target.getTexture().getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, simWidth, simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

template <class ShaderClass>
void GpuHost<ShaderClass>::OnRender(Scene& scene)
{
	scene.ActivateScreenSpaceDrawing();
	scene.Submit(sf::Sprite(_target.getTexture()), sf::RenderStates::Default);	
	scene.DeactivateScreenSpaceDrawing();
}

template<class ShaderClass>
void GpuHost<ShaderClass>::RenderImage()
{
	if (_painterPS == nullptr)
	{
		Log::CoreError("Cannot render GPU host without painter shader");
		return;
	}

	const auto& palTex = PaletteManager::Instance().Texture();

	glBindImageTexture(0, TextureHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(1, palTex.getNativeHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	SetUniform(_painterPS->getNativeHandle(), "maxPixelValue", static_cast<float>(this->ComputeIterations()));
	SetUniform(_painterPS->getNativeHandle(), "paletteWidth", PaletteManager::PaletteWidth);
	
	sf::RectangleShape simRectShape(sf::Vector2f(SimWidth(), SimHeight()));
	simRectShape.setTexture(&palTex);
	_target.draw(simRectShape, { _painterPS.get() });
	
}

template <class ShaderClass>
void GpuHost<ShaderClass>::Resize(int width, int height)
{
	const auto result = TryResizeTarget(width, height);
	if (!result)
	{
		Log::CoreError(result.error().message);
	}
}

template <class ShaderClass>
auto GpuHost<ShaderClass>::TryResizeTarget(int width, int height) -> Status
{
	auto painter = ShaderStore::TryGet("painter.frag", sf::Shader::Type::Fragment);
	if (!painter)
	{
		return std::unexpected(painter.error());
	}

	_painterPS = *painter;
	if (!_target.create(width, height))
	{
		return std::unexpected(Error{ErrorCode::Graphics,
		                            "Failed to create GPU render target " + std::to_string(width) + "x" +
		                                std::to_string(height)});
	}

	return {};
}
}
