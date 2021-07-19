#pragma once

#include <complex>

#include <Saffron.h>

#include "FractalSet.h"

namespace Se
{
enum class JuliaState
{
	Animate,
	FollowCursor,
	None
};

typedef uint JuliaDrawFlags;

enum JuliaDrawFlags_ : uint
{
	JuliaDrawFlags_None = 0u,
	JuliaDrawFlags_Dot = 1u << 0u,
	JuliaDrawFlags_All = 0xffffffff
};

class Julia : public FractalSet
{
public:
	explicit Julia(const sf::Vector2f& renderSize);
	~Julia() override = default;

	void OnUpdate(Scene& scene) override;
	void OnRender(Scene& scene) override;

	const Complex<double>& C() const noexcept;
	JuliaDrawFlags DrawFlags() const;
	
	void SetState(JuliaState state) noexcept;
	void SetDrawFlags(JuliaDrawFlags flags);
	void SetC(const Complex<double>& c, bool animate = false);
	void SetCR(double r, bool animate = false);
	void SetCI(double i, bool animate = false);

private:
	auto ComputeShader() -> Shared<class ComputeShader> override;
	void UpdateComputeShaderUniforms() override;

	auto PixelShader() -> Shared<sf::Shader> override;
	void UpdatePixelShaderUniforms() override;

private:
	Shared<class ComputeShader> _computeCS;
	Shared<sf::Shader> _pixelShader;

	JuliaState _state;
	JuliaDrawFlags _drawFlags = JuliaDrawFlags_None;

	Complex<double> _desiredC;
	Complex<double> _currentC;
	Complex<double> _startC;

	float _animationTimer;

	float _cTransitionTimer;
	float _cTransitionDuration;

private:
	struct JuliaWorker : Worker
	{
		~JuliaWorker() override = default;
		void Compute() override;

		Complex<double> C;
	};
};
}
