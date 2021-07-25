#pragma once

#include <complex>

#include <Saffron.h>

#include "FractalSet.h"
#include "ComputeHosts/CpuHost.h"
#include "ComputeHosts/ComputeShaderHost.h"
#include "ComputeHosts/PixelShaderHost.h"

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
	JuliaDrawFlags_None= 0u,
	JuliaDrawFlags_Dot = 1u << 0u,
	JuliaDrawFlags_ComplexLines = 1u << 1u,
	JuliaDrawFlags_All = 0xffffffff
};

class Julia : public FractalSet
{
public:
	explicit Julia(const sf::Vector2f& renderSize);
	~Julia() override = default;

	void OnUpdate(Scene& scene) override;
	void OnRender(Scene& scene) override;
	void OnViewportResize(const sf::Vector2f& size) override;

	void ResumeAnimation();
	void PauseAnimation();
	auto Paused() const -> bool;

	const std::complex<double>& C() const noexcept;
	JuliaDrawFlags DrawFlags() const;

	void SetState(JuliaState state) noexcept;
	void SetDrawFlags(JuliaDrawFlags flags);
	void SetC(const std::complex<double>& c, bool animate = false);
	void SetCr(double r, bool animate = false);
	void SetCi(double i, bool animate = false);

	auto TranslatePoint(const sf::Vector2f& point, int iterations) -> sf::Vector2f;

private:
	void UpdateComputeShaderUniforms(ComputeShader& shader);
	void UpdatePixelShaderUniforms(sf::Shader& shader);

private:
	JuliaState _state = JuliaState::None;
	JuliaDrawFlags _drawFlags = JuliaDrawFlags_None;

	std::complex<double> _desiredC;
	std::complex<double> _currentC;
	std::complex<double> _startC;

	float _animationTimer = 0.0f;
	bool _animPaused = false;

	float _cTransitionTimer = 0.0f;
	float _cTransitionDuration = 0.5f;

private:
	struct JuliaWorker : Worker
	{
		void Compute() override;

		std::complex<double> C;
	};
};
}
