#include "Mandelbrot.h"

#include <glad/glad.h>

#include <Saffron/Core/SIMD.h>

#include "ComputeHosts/CpuHost.h"
#include "ComputeHosts/ComputeShaderHost.h"
#include "ComputeHosts/PixelShaderHost.h"

namespace Se
{
Mandelbrot::Mandelbrot(const sf::Vector2f& renderSize) :
	FractalSet("Mandelbrot", FractalSetType::Mandelbrot, renderSize),
	_drawFlags(MandelbrotDrawFlags_None)
{
	const auto x = renderSize.x, y = renderSize.y;

	auto cpuHost = std::make_unique<CpuHost>(x, y);
	auto comHost = std::make_unique<ComputeShaderHost>("mandelbrot.comp", x, y, sf::Vector2u(x, y));
	auto pixHost = std::make_unique<PixelShaderHost>("mandelbrot.frag", x, y);

	for (int i = 0; i < 32; i++)
	{
		cpuHost->AddWorker(std::make_unique<MandelbrotWorker>());
	}

	comHost->RequestUniformUpdate += [this](ComputeShader& shader)
	{
		UpdateComputeShaderUniforms(shader);
		return false;
	};

	pixHost->RequestUniformUpdate += [this](sf::Shader& shader)
	{
		UpdatePixelShaderUniforms(shader);
		return false;
	};

	AddHost(std::move(cpuHost));
	AddHost(std::move(comHost));
	AddHost(std::move(pixHost));

	_places.push_back({"Elephant Valley", {0.3, 0.0}, 700});
}

auto Mandelbrot::DrawFlags() const -> MandelbrotDrawFlags
{
	return _drawFlags;
}

void Mandelbrot::SetDrawFlags(MandelbrotDrawFlags state) noexcept
{
	_drawFlags = state;
}

auto Mandelbrot::TranslatePoint(const sf::Vector2f& point, int iterations) -> sf::Vector2f
{
	const std::complex<double> c(point.x, point.y);
	std::complex z(0.0, 0.0);

	for (int n = 0; n < iterations && abs(z) < 2.0; n++)
	{
		z = (z * z) + c;
	}

	return sf::Vector2f(z.real(), z.imag());
}

void Mandelbrot::OnRender(Scene& scene)
{
	FractalSet::OnRender(scene);

	if (_drawFlags & MandelbrotDrawFlags_ComplexLines)
	{
		const sf::Vector2f start = scene.Camera().ScreenToWorld(scene.ViewportPane().MousePosition());
		sf::Vector2f to = start;
		for (int i = 1; i < _computeIterations; i++)
		{
			sf::Vector2f from = TranslatePoint(start, i);
			scene.Submit(from, to, sf::Color(200, 200, 200, 60));
			to = from;
			scene.Submit(to, sf::Color(255, 255, 255, 150), 5.0f);
		}
	}
}

void Mandelbrot::OnViewportResize(const sf::Vector2f& size)
{
	FractalSet::OnViewportResize(size);
	const auto sizeU = VecUtils::ConvertTo<sf::Vector2u>(size);
	_hosts.at(HostType::GpuComputeShader)->As<ComputeShaderHost>().SetDimensions(sizeU);
}

void Mandelbrot::UpdateComputeShaderUniforms(ComputeShader& shader)
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);
	shader.SetVector2d("fractalTL", _simBox.TopLeft);
	shader.SetDouble("xScale", xScale);
	shader.SetDouble("yScale", yScale);
	shader.SetInt("iterations", _computeIterations);
}

void Mandelbrot::UpdatePixelShaderUniforms(sf::Shader& shader)
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);

	SetUniform(shader.getNativeHandle(), "fractalTL", _simBox.TopLeft);
	SetUniform(shader.getNativeHandle(), "xScale", xScale);
	SetUniform(shader.getNativeHandle(), "yScale", yScale);
	SetUniform(shader.getNativeHandle(), "iterations", static_cast<int>(_computeIterations));
}

void Mandelbrot::MandelbrotWorker::Compute()
{
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

		double xScale = (FractalBR.x - FractalTL.x) / (ImageBR.x - ImageTL.x);
		double yScale = (FractalBR.y - FractalTL.y) / (ImageBR.y - ImageTL.y);

		double yPos = FractalTL.y;

		int yOffset = 0;
		int rowSize = SimWidth;

		int x, y;

		SIMD_Double a, b, two, four, mask1;
		SIMD_Double zr, zi, zr2, zi2, cr, ci;
		SIMD_Double xPosOffsets, xPos, xScaleSimd, xJump;
		SIMD_Integer one, c, n, iterations, mask2;

		one = SIMD_SetOnei(1);
		two = SIMD_SetOne(2.0);
		four = SIMD_SetOne(4.0);
		iterations = SIMD_SetOnei(Iterations);

		xScaleSimd = SIMD_SetOne(xScale);
		xJump = SIMD_SetOne(xScale * 4.0);
		xPosOffsets = SIMD_Set(0.0, 1.0, 2.0, 3.0);
		xPosOffsets = SIMD_Mul(xPosOffsets, xScaleSimd);

		for (y = ImageTL.y; y < ImageBR.y; y++)
		{
			// Reset x_position
			a = SIMD_SetOne(FractalTL.x);
			xPos = SIMD_Add(a, xPosOffsets);

			ci = SIMD_SetOne(yPos);

			for (x = ImageTL.x; x < ImageBR.x; x += 4)
			{
				cr = xPos;
				zr = SIMD_SetZero();
				zi = SIMD_SetZero();
				n = SIMD_SetZero256i();

			repeat: zr2 = SIMD_Mul(zr, zr);
				zi2 = SIMD_Mul(zi, zi);
				a = SIMD_Sub(zr2, zi2);
				a = SIMD_Add(a, cr);
				b = SIMD_Mul(zr, zi);
				b = SIMD_Mul(b, two);
				b = SIMD_Add(b, ci);
				zr = a;
				zi = b;
				a = SIMD_Add(zr2, zi2);
				mask1 = SIMD_LessThan(a, four);
				mask2 = SIMD_GreaterThani(iterations, n);
				mask2 = SIMD_Andi(mask2, SIMD_CastToInt(mask1));
				c = SIMD_Andi(one, mask2); // Zero out ones where n < iterations
				n = SIMD_Addi(n, c); // n++ Increase all n
				if (SIMD_SignMask(SIMD_CastToFloat(mask2)) > 0) goto repeat;

#if defined(__MINGW32__)
				fractalArray[y_offset + x + 0] = static_cast<int>(_n[3]);
				fractalArray[y_offset + x + 1] = static_cast<int>(_n[2]);
				fractalArray[y_offset + x + 2] = static_cast<int>(_n[1]);
				fractalArray[y_offset + x + 3] = static_cast<int>(_n[0]);
#elif defined (_MSC_VER)
				FractalArray[yOffset + x + 0] = static_cast<int>(n.m256i_i64[3]);
				FractalArray[yOffset + x + 1] = static_cast<int>(n.m256i_i64[2]);
				FractalArray[yOffset + x + 2] = static_cast<int>(n.m256i_i64[1]);
				FractalArray[yOffset + x + 3] = static_cast<int>(n.m256i_i64[0]);
#endif

				xPos = SIMD_Add(xPos, xJump);
			}


			yPos += yScale;
			yOffset += rowSize;
		}
		++(*WorkerComplete);
	}
}
}
