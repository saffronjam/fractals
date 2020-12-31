#pragma once

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

	const std::complex<double> &GetC() const noexcept { return _desiredC; }

	void SetState(State state) noexcept { _state = state; }
	void SetC(const std::complex<double> &c, bool animate = false);
	void SetCR(double r, bool animate = false) { SetC(std::complex(r, GetC().imag()), animate); }
	void SetCI(double i, bool animate = false) { SetC(std::complex(GetC().real(), i), animate); }

private:
	State _state;

	std::complex<double> _desiredC;
	std::complex<double> _currentC;
	std::complex<double> _startC;

	float _animationTimer;

	float _cTransitionTimer;
	float _cTransitionDuration;

private:
	struct JuliaWorker : public FractalSet::Worker
	{
		~JuliaWorker() override = default;
		void Compute() override;

		std::complex<double> c;
	};
};
}