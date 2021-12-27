#include "ComputeHosts/PixelShaderHost.h"

#include "glad/glad.h"

#include "PaletteManager.h"

namespace Se
{
PixelShaderHost::PixelShaderHost(const std::filesystem::path& pixelShaderPath, int simWidth, int simHeight) :
	GpuHost(HostType::GpuPixelShader, "GPU Pixel Shader", simWidth, simHeight),
	_shader(ShaderStore::Get(pixelShaderPath, sf::Shader::Type::Fragment))
{
	_output.create(simWidth, simHeight);
}

void PixelShaderHost::ComputeImage()
{
	RequestUniformUpdate.Invoke(*_shader);
	sf::RectangleShape simRectShape(sf::Vector2f(SimWidth(), SimHeight()));
	simRectShape.setTexture(&PaletteManager::Instance().Texture());
	
	_output.draw(simRectShape, {_shader.get()});
}

void PixelShaderHost::Resize(int width, int height)
{
	GpuHost::Resize(width, height);
	
	_output.create(width, height);
	glBindTexture(GL_TEXTURE_2D, _output.getTexture().getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

auto PixelShaderHost::TextureHandle() const -> uint
{
	return _output.getTexture().getNativeHandle();
}
}
