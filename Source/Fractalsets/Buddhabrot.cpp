#include "Buddhabrot.h"

#include <glad/glad.h>

#include "ComputeHosts/ComputeShaderHost.h"

namespace Se
{
Buddhabrot::Buddhabrot(const sf::Vector2f& renderSize) :
	FractalSet("Buddhabrot", FractalSetType::Buddhabrot, renderSize)
{
	_activeHost = HostType::GpuComputeShader;
	_generationType = FractalSetGenerationType::DelayedGeneration;

	const auto nPoints = static_cast<size_t>(50000);
	std::generate_n(std::back_inserter(_points), nPoints, []
	{
		return Random::Vec2(0.0, 0.0, 1.0, 1.0);
	});

	glGenBuffers(1, &_ssbo);

	const auto alloc = nPoints * sizeof(Position);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, alloc, _points.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


	const auto x = renderSize.x, y = renderSize.y;

	auto comHost = std::make_unique<ComputeShaderHost>("buddhabrot.comp", x, y, sf::Vector2u(x, y));

	comHost->RequestUniformUpdate += [this](ComputeShader& computeShader)
	{
		UpdateComputeShaderUniforms(computeShader);
		return false;
	};

	AddHost(std::move(comHost));

	// Todo: Fix this
	/*for (int i = 0; i < 32; i++)
	{
		AddWorker(new BuddhabrotWorker);
	}*/
}

auto Buddhabrot::TranslatePoint(const sf::Vector2f& point, int iterations) -> sf::Vector2f
{
	const std::complex<double> c(point.x, point.y);
	std::complex z(0.0, 0.0);

	for (int n = 0; n < iterations && abs(z) < 2.0; n++)
	{
		z = (z * z) + c;
	}

	return sf::Vector2f(z.real(), z.imag());
}

void Buddhabrot::OnRender(Scene& scene)
{
	FractalSet::OnRender(scene);
}

void Buddhabrot::OnViewportResize(const sf::Vector2f& size)
{
	FractalSet::OnViewportResize(size);
	const auto sizeU = VecUtils::ConvertTo<sf::Vector2u>(size);
	_hosts.at(HostType::GpuComputeShader)->As<ComputeShaderHost>().SetDimensions(sizeU);
}

void Buddhabrot::UpdateComputeShaderUniforms(ComputeShader& shader)
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);
	shader.SetVector2d("fractalTL", _simBox.TopLeft);
	shader.SetDouble("xScale", xScale);
	shader.SetDouble("yScale", yScale);
	shader.SetInt("iterations", _computeIterations);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _ssbo);
}

void Buddhabrot::BuddhabrotWorker::Compute()
{
	return;
	// Todo: Fix this
	while (Alive)
	{
		std::unique_lock lm(Mutex);
		CvStart.wait(lm);
		if (!Alive)
		{
			++(*WorkerComplete);
			return;
		}
		// Somehow declaring this variable fixes a race condition
		int bugfixer = 0;

		const double xScale = (FractalBR.x - FractalTL.x) / (ImageBR.x - ImageTL.x);
		const double yScale = (FractalBR.y - FractalTL.y) / (ImageBR.y - ImageTL.y);

		auto mandelbrotIterations = [this](const Position& fractalCoord)
		{
			Position z;
			const Position c = fractalCoord;

			int n = 0;
			for (double result = 0.0; n <= Iterations && result < 4.0; n++)
			{
				const double zRealTmp = z.x;
				z.x = (zRealTmp * zRealTmp) - (z.y * z.y) + c.x;
				z.y = 2.0 * zRealTmp * z.y + c.y;
				result = VecUtils::Dot(z, z);
			}
			return n;
		};

		auto mandelbrotRecord = [this, xScale, yScale](const Position& fractalCoord)
		{
			Position z;
			const Position c = fractalCoord;

			for (int n = 0; n < Iterations && VecUtils::Dot(z, z) < 4.0; n++)
			{
				const double zRealTmp = z.x;
				z.x = (zRealTmp * zRealTmp) - (z.y * z.y) + c.x;
				z.y = 2.0 * zRealTmp * z.y + c.y;


				const int x = (z.x - FractalTL.x) / xScale;
				const int y = (z.y - FractalTL.y) / yScale;

				const auto index = SimWidth * y + x;

				if (index >= 0 && index < ImageBR.y * SimWidth)
				{
					FractalArray[index] = 10;
				}
			}
		};

		for (int y = ImageTL.y; y < ImageBR.y; y++)
		{
			for (int x = ImageTL.x; x < ImageBR.x; x += 4)
			{
				const Position coord(static_cast<double>(x) * xScale, static_cast<double>(y) * yScale);

				const auto iterations = mandelbrotIterations(coord);
				if (iterations >= Iterations) continue;

				mandelbrotRecord(coord);


				const int highestIndex = ImageBR.y * SimWidth;
			}
		}
		++(*WorkerComplete);
	}
}
}
