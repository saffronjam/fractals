#pragma once

#include <memory>

#include "compute_hosts/gpu_host.h"

namespace fractals {
using namespace saffron;
class PixelShaderHost : public GpuHost<sf::Shader> {
  public:
    PixelShaderHost(const std::filesystem::path& pixelShaderPath, int simWidth, int simHeight);

  protected:
    void ComputeImage() override;
    void Resize(int width, int height) override;

    auto TextureHandle() const -> uint override;

  private:
    std::shared_ptr<sf::Shader> _shader;
    sf::RenderTexture _output;
};
} // namespace fractals
