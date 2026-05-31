#pragma once

#include "compute_hosts/cpu_host.h"
#include "fractal_set.h"

namespace fractals {
using namespace saffron;
class Buddhabrot : public FractalSet {
  public:
    explicit Buddhabrot(const sf::Vector2f& renderSize);
    ~Buddhabrot() override = default;

    void OnRender(Scene& scene) override;
    void OnViewportResize(const sf::Vector2f& size) override;

    static auto TranslatePoint(const sf::Vector2f& point, int iterations) -> sf::Vector2f;

  private:
    void UpdateComputeShaderUniforms(ComputeShader& shader);

  private:
    uint _ssbo;

    float _pointCoverage = 50.0f;
    std::vector<Position> _points;

  private:
    struct BuddhabrotWorker : Worker {
        void Compute() override;
    };
};
} // namespace fractals
