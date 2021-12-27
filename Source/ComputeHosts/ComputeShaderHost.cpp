#include "ComputeHosts/ComputeShaderHost.h"

#include "glad/glad.h"

namespace Se
{
ComputeShaderHost::ComputeShaderHost(const std::filesystem::path& computeShaderPath, int simWidth, int simHeight,
                                     sf::Vector2u dimensions) :
	GpuHost("GPU Compute Shader", simWidth, simHeight),
	_shader(ComputeShaderStore::Get(computeShaderPath)),
	_dimensions(dimensions)
{
	_output.create(simWidth, simHeight);
	glBindTexture(GL_TEXTURE_2D, _output.getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, simWidth, simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ComputeShaderHost::OnViewportResize(const sf::Vector2f& size)
{
	GpuHost::OnViewportResize(size);
	_blackColorCache.resize(size.x * size.y);
	for (auto& color : _blackColorCache)
	{
		color = sf::Color::Black;
	}
}

auto ComputeShaderHost::Dimensions() const -> const sf::Vector2u&
{
	return _dimensions;
}

void ComputeShaderHost::SetDimensions(const sf::Vector2u dimensions)
{
	_dimensions = dimensions;
}

void ComputeShaderHost::ComputeImage()
{
	// Clears texture
	glBindTexture(GL_TEXTURE_2D, _output.getNativeHandle());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SimWidth(), SimHeight(), GL_RGBA, GL_UNSIGNED_BYTE,
	                _blackColorCache.data());
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindImageTexture(0, _output.getNativeHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	RequestUniformUpdate.Invoke(*_shader);

	_shader->Dispatch(_dimensions.x, _dimensions.y, 1);
	ComputeShader::AwaitFinish();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ComputeShaderHost::Resize(int width, int height)
{
	GpuHost::Resize(width, height);
	if (sf::Vector2u(width, height) != _output.getSize())
	{
		_output.create(width, height);

		glBindTexture(GL_TEXTURE_2D, _output.getNativeHandle());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, _target.getTexture().getNativeHandle());
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

auto ComputeShaderHost::TextureHandle() const -> uint
{
	return _output.getNativeHandle();
}
}
