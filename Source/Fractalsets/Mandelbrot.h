#pragma once

#include <Saffron.h>

#include "FractalSet.h"

namespace Se
{
class Mandelbrot : public FractalSet
{
public:
	enum class State
	{
		ComplexLines,
		None
	};

public:
	explicit Mandelbrot(const sf::Vector2f& renderSize);
	~Mandelbrot() override = default;

	void OnRender(Scene& scene) override;

	static auto TranslatePoint(const sf::Vector2f& point, int iterations) -> sf::Vector2f;

	void SetState(State state) noexcept { _state = state; }

private:
	auto ComputeShader() -> Shared<class ComputeShader> override;
	void UpdateComputeShaderUniforms() override;

	auto PixelShader() -> Shared<sf::Shader> override;
	void UpdatePixelShaderUniforms() override;

private:
	Shared<class ComputeShader> _computeCS;
	Shared<sf::Shader> _pixelShader;

	State _state;

private:
	struct MandelbrotWorker : Worker
	{
		~MandelbrotWorker() override = default;
		void Compute() override;
	};
};
}
