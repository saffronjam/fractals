#pragma once

#include "ComputeHosts/GpuHost.h"

namespace Se
{
class ComputeShaderHost : public GpuHost<ComputeShader>
{
public:
	ComputeShaderHost(const std::filesystem::path& computeShaderPath, int simWidth, int simHeight, sf::Vector2u dimensions);

	void OnViewportResize(const sf::Vector2f& size) override;

	auto Dimensions() const -> const sf::Vector2u&;
	void SetDimensions(const sf::Vector2u dimensions);

private:
	void ComputeImage() override;
	void Resize(int width, int height) override;

	auto TextureHandle() const -> uint override;

private:
	std::shared_ptr<class ComputeShader> _shader;
	sf::Vector2u _dimensions;
	std::vector<sf::Color> _blackColorCache;
	
	sf::Texture _output;
};
}
