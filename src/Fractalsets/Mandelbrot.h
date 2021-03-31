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
	explicit Mandelbrot(const sf::Vector2f &renderSize);
	~Mandelbrot() override = default;

	void OnRender(Scene &scene) override;


	static sf::Vector2f TranslatePoint(const sf::Vector2f &point, int iterations);

	void SetState(State state) noexcept { _state = state; }

private:	
	Shared<ComputeShader> GetComputeShader() override;
	void UpdateComputeShaderUniforms() override;

private:
	Shared<ComputeShader> _computeCS;
	
	State _state;

private:
	struct MandelbrotWorker : public FractalSet::Worker
	{
		~MandelbrotWorker() override = default;
		void Compute() override;
	};
};
}