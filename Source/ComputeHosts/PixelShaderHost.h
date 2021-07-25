#pragma once

#include "ComputeHosts/GpuHost.h"

namespace Se
{
class PixelShaderHost : public GpuHost<sf::Shader>
{
public:
	PixelShaderHost(const Path& pixelShaderPath, int simWidth, int simHeight);

protected:
	void ComputeImage() override;
	void Resize(int width, int height) override;

	auto TextureHandle() const -> uint override;

private:
	Shared<sf::Shader> _shader;
	sf::RenderTexture _output;
};
}
