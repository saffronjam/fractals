#pragma once

#include "FractalSet.h"

namespace Se
{
class Buddhabrot : public FractalSet
{
public:
	explicit Buddhabrot(const sf::Vector2f& renderSize);
	~Buddhabrot() override = default;

	void OnRender(Scene& scene) override;
	void OnViewportResize(const sf::Vector2f& size) override;

	static auto TranslatePoint(const sf::Vector2f& point, int iterations) -> sf::Vector2f;

private:
	auto ComputeShader() -> Shared<class ComputeShader> override;
	void UpdateComputeShaderUniforms() override;
	sf::Vector2u ComputeShaderWorkerDim() override;

	auto PixelShader() -> Shared<sf::Shader> override;
	void UpdatePixelShaderUniforms() override;

private:
	Shared<class ComputeShader> _computeCS;
	uint _ssbo;

	float _pointCoverage = 50.0f;
	List<Position> _points;

private:
	struct BuddhabrotWorker : Worker
	{
		~BuddhabrotWorker() override = default;
		void Compute() override;
	};
};
}
