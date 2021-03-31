#pragma once

#include <complex>

#include <Saffron.h>

#include "FractalSet.h"

namespace Se
{
class Julia : public FractalSet
{
public:
	enum class State
	{
		Animate,
		FollowCursor,
		None
	};

public:
	explicit Julia(const sf::Vector2f &renderSize);
	~Julia() override = default;

	void OnUpdate(Scene &scene) override;

	Shared<ComputeShader> GetComputeShader() override;
	
	const Complex<double>& GetC() const noexcept;

	void SetState(State state) noexcept;
	void SetC(const Complex<double> &c, bool animate = false);
	void SetCR(double r, bool animate = false);

	void SetCI(double i, bool animate = false);
	

private:
	Shared<ComputeShader> _computeCS;
	
	State _state;

	Complex<double> _desiredC;
	Complex<double> _currentC;
	Complex<double> _startC;

	float _animationTimer;

	float _cTransitionTimer;
	float _cTransitionDuration;

private:
	struct JuliaWorker : public FractalSet::Worker
	{
		~JuliaWorker() override = default;
		void Compute() override;

		Complex<double> c;
	};
};
}