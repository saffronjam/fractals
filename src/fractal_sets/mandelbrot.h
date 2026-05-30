#pragma once

#include <saffron.h>

#include "fractal_set.h"
#include "compute_hosts/cpu_host.h"

namespace fractals
{
using namespace saffron;
typedef uint MandelbrotDrawFlags;

enum MandelbrotDrawFlags_ : uint
{
	MandelbrotDrawFlags_None = 0u,
	MandelbrotDrawFlags_ComplexLines = 1u << 0u,
	MandelbrotDrawFlags_All = 0xffffffff
};

class Mandelbrot : public FractalSet
{
public:
	explicit Mandelbrot(const sf::Vector2f& renderSize);
	~Mandelbrot() override = default;

	void OnRender(Scene& scene) override;
	void OnViewportResize(const sf::Vector2f& size) override;

	auto DrawFlags() const -> MandelbrotDrawFlags;
	void SetDrawFlags(MandelbrotDrawFlags state) noexcept;

	static auto TranslatePoint(const sf::Vector2f& point, int iterations)->sf::Vector2f;

private:
	void UpdateComputeShaderUniforms(ComputeShader& shader);
	void UpdatePixelShaderUniforms(sf::Shader& shader);

private:

	MandelbrotDrawFlags _drawFlags;

private:
	struct MandelbrotWorker : Worker
	{
		void Compute() override;
	};
};
}
