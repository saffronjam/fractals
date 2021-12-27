#pragma once

#include "FractalSet.h"

namespace Se
{
class Polynomial : public FractalSet
{
public:
	static constexpr int PolynomialDegree = 4;

public:
	explicit Polynomial(const sf::Vector2f& renderSize);

	void OnUpdate(Scene& scene) override;
	void OnRender(Scene& scene) override;
	void OnViewportResize(const sf::Vector2f& size) override;

	auto Constants() const -> const std::array<double, PolynomialDegree>&;
	void SetConstants(const std::array<double, PolynomialDegree>& constants, bool animate = false);

private:
	void UpdatePixelShaderUniforms(sf::Shader& shader);

private:
	std::array<double, PolynomialDegree> _constants{};
	std::array<double, PolynomialDegree> _desiredConstants{};
	std::array<double, PolynomialDegree> _startConstants{};

	float _cTransitionTimer = 0.0f;
	float _cTransitionDuration = 0.5f;

	// Save here if it will be changed in the future, probably not...
	std::array<double, PolynomialDegree> _exponents{};
};
}
