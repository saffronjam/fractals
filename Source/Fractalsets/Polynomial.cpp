#include "Polynomial.h"

#include "ComputeHosts/PixelShaderHost.h"

namespace Se
{
Polynomial::Polynomial(const sf::Vector2f& renderSize) :
	FractalSet("Polynomial", FractalSetType::Polynomial, renderSize),
	_constants({1, 0, -2, 2}),
	_desiredConstants(_constants),
	_startConstants(_constants),
	_exponents({3, 2, 1, 0})
{
	_activeHost = HostType::GpuPixelShader;
	_generationType = FractalSetGenerationType::AutomaticGeneration;

	const auto x = renderSize.x, y = renderSize.y;

	auto pixHost = std::make_unique<PixelShaderHost>("custom.frag", x, y);

	pixHost->RequestUniformUpdate += [this](sf::Shader& shader)
	{
		UpdatePixelShaderUniforms(shader);
		return false;
	};

	AddHost(std::move(pixHost));
}

void Polynomial::OnUpdate(Scene& scene)
{
	bool allMatch = true;
	for (int i = 0; i < PolynomialDegree; i++)
	{
		if (_constants[i] != _desiredConstants[i])
		{
			allMatch = false;
			break;
		}
	}
	if (!allMatch)
	{
		RequestImageComputation();
		RequestImageRendering();
	}


	if (_cTransitionTimer <= _cTransitionDuration)
	{
		const float delta = (std::sin((_cTransitionTimer / _cTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) / 2.0f;
		for (int i = 0; i < PolynomialDegree; i++)
		{
			_constants[i] = _startConstants[i] + static_cast<double>(delta) * (_desiredConstants[i] - _startConstants[
				i]);
		}
		_cTransitionTimer += Global::Clock::FrameTime().asSeconds();
	}
	else
	{
		_constants = _desiredConstants;
	}

	FractalSet::OnUpdate(scene);
}

void Polynomial::OnRender(Scene& scene)
{
	FractalSet::OnRender(scene);
}

void Polynomial::OnViewportResize(const sf::Vector2f& size)
{
	FractalSet::OnViewportResize(size);
}

auto Polynomial::Constants() const -> const std::array<double, PolynomialDegree>&
{
	return _constants;
}

void Polynomial::SetConstants(const std::array<double, PolynomialDegree>& constants, bool animate)
{
	if (animate)
	{
		_startConstants = _constants;
		_cTransitionTimer = 0.0f;
	}
	else
	{
		RequestImageComputation();
		RequestImageRendering();
	}
	_constants = constants;
	_desiredConstants = constants;
}

void Polynomial::UpdatePixelShaderUniforms(sf::Shader& shader)
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);

	SetUniform(shader.getNativeHandle(), "fractalTL", _simBox.TopLeft);
	SetUniform(shader.getNativeHandle(), "xScale", xScale);
	SetUniform(shader.getNativeHandle(), "yScale", yScale);
	SetUniform(shader.getNativeHandle(), "iterations", static_cast<int>(_computeIterations));

	SetUniform(shader.getNativeHandle(), "constants", _constants);
	SetUniform(shader.getNativeHandle(), "exponents", _exponents);
}
}
