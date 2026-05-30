#include "compute_hosts/pixel_shader_host.h"

#include "glad/glad.h"

#include "palette_manager.h"

namespace fractals
{
using namespace saffron;
PixelShaderHost::PixelShaderHost(const std::filesystem::path& pixelShaderPath, int simWidth, int simHeight) :
	GpuHost(HostType::GpuPixelShader, "GPU Pixel Shader", simWidth, simHeight)
{
	auto shader = ShaderStore::TryGet(pixelShaderPath, sf::Shader::Type::Fragment);
	if (!shader)
	{
		Log::CoreError(shader.error().message);
		return;
	}
	_shader = *shader;
	if (!_output.create(simWidth, simHeight))
	{
		Log::CoreError("Failed to create pixel shader output texture {0}x{1}", simWidth, simHeight);
		_shader = nullptr;
	}
}

void PixelShaderHost::ComputeImage()
{
	if (_shader == nullptr)
	{
		return;
	}

	RequestUniformUpdate.Invoke(*_shader);
	sf::RectangleShape simRectShape(sf::Vector2f(SimWidth(), SimHeight()));
	simRectShape.setTexture(&PaletteManager::Instance().Texture());
	
	_output.draw(simRectShape, {_shader.get()});
}

void PixelShaderHost::Resize(int width, int height)
{
	GpuHost::Resize(width, height);
	
	if (!_output.create(width, height))
	{
		Log::CoreError("Failed to resize pixel shader output texture {0}x{1}", width, height);
		return;
	}
	glBindTexture(GL_TEXTURE_2D, _output.getTexture().getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

auto PixelShaderHost::TextureHandle() const -> uint
{
	return _output.getTexture().getNativeHandle();
}
}
