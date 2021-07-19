#include "Mandelbrot.h"

#include <glad/glad.h>

#include <Saffron/Core/SIMD.h>

namespace Se
{
Mandelbrot::Mandelbrot(const sf::Vector2f& renderSize) :
	FractalSet("Mandelbrot", FractalSetType::Mandelbrot, renderSize),
	_computeCS(ComputeShaderStore::Get("mandelbrot.comp")),
	_pixelShader(ShaderStore::Get("mandelbrot.frag", sf::Shader::Fragment)),
	_state(State::None)
{
	for (int i = 0; i < 32; i++)
	{
		AddWorker(new MandelbrotWorker);
	}
}

auto Mandelbrot::TranslatePoint(const sf::Vector2f& point, int iterations) -> sf::Vector2f
{
	const Complex<double> c(point.x, point.y);
	Complex<double> z(0.0, 0.0);

	for (int n = 0; n < iterations && abs(z) < 2.0; n++)
	{
		z = (z * z) + c;
	}

	return sf::Vector2f(z.real(), z.imag());
}

void Mandelbrot::OnRender(Scene& scene)
{
	FractalSet::OnRender(scene);
	if (_state == State::ComplexLines)
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

auto Mandelbrot::ComputeShader() -> Shared<class ComputeShader>
{
	return _computeCS;
}

void Mandelbrot::UpdateComputeShaderUniforms()
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);
	_computeCS->SetVector2d("fractalTL", _simBox.TopLeft);
	_computeCS->SetDouble("xScale", xScale);
	_computeCS->SetDouble("yScale", yScale);
	_computeCS->SetInt("iterations", _computeIterations);
}

auto Mandelbrot::PixelShader() -> Shared<sf::Shader>
{
	return _pixelShader;
}

void Mandelbrot::UpdatePixelShaderUniforms()
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);

	SetUniform(_pixelShader->getNativeHandle(), "fractalTL", _simBox.TopLeft);
	SetUniform(_pixelShader->getNativeHandle(), "xScale", xScale);
	SetUniform(_pixelShader->getNativeHandle(), "yScale", yScale);
	SetUniform(_pixelShader->getNativeHandle(), "iterations", static_cast<int>(_computeIterations));
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

		double y_pos = FractalTL.y;

		int y_offset = 0;
		int row_size = SimWidth;

		int x, y;

		SIMD_Double _a, _b, _two, _four, _mask1;
		SIMD_Double _zr, _zi, _zr2, _zi2, _cr, _ci;
		SIMD_Double _x_pos_offsets, _x_pos, _x_scale, _x_jump;
		SIMD_Integer _one, _c, _n, _iterations, _mask2;

		_one = SIMD_SetOnei(1);
		_two = SIMD_SetOne(2.0);
		_four = SIMD_SetOne(4.0);
		_iterations = SIMD_SetOnei(Iterations);

		_x_scale = SIMD_SetOne(xScale);
		_x_jump = SIMD_SetOne(xScale * 4.0);
		_x_pos_offsets = SIMD_Set(0.0, 1.0, 2.0, 3.0);
		_x_pos_offsets = SIMD_Mul(_x_pos_offsets, _x_scale);

		for (y = ImageTL.y; y < ImageBR.y; y++)
		{
			// Reset x_position
			_a = SIMD_SetOne(FractalTL.x);
			_x_pos = SIMD_Add(_a, _x_pos_offsets);

			_ci = SIMD_SetOne(y_pos);

			for (x = ImageTL.x; x < ImageBR.x; x += 4)
			{
				_cr = _x_pos;
				_zr = SIMD_SetZero();
				_zi = SIMD_SetZero();
				_n = SIMD_SetZero256i();

			repeat: _zr2 = SIMD_Mul(_zr, _zr);
				_zi2 = SIMD_Mul(_zi, _zi);
				_a = SIMD_Sub(_zr2, _zi2);
				_a = SIMD_Add(_a, _cr);
				_b = SIMD_Mul(_zr, _zi);
				_b = SIMD_Mul(_b, _two);
				_b = SIMD_Add(_b, _ci);
				_zr = _a;
				_zi = _b;
				_a = SIMD_Add(_zr2, _zi2);
				_mask1 = SIMD_LessThan(_a, _four);
				_mask2 = SIMD_GreaterThani(_iterations, _n);
				_mask2 = SIMD_Andi(_mask2, SIMD_CastToInt(_mask1));
				_c = SIMD_Andi(_one, _mask2); // Zero out ones where n < iterations
				_n = SIMD_Addi(_n, _c); // n++ Increase all n
				if (SIMD_SignMask(SIMD_CastToFloat(_mask2)) > 0) goto repeat;

#if defined(__MINGW32__)
				fractalArray[y_offset + x + 0] = static_cast<int>(_n[3]);
				fractalArray[y_offset + x + 1] = static_cast<int>(_n[2]);
				fractalArray[y_offset + x + 2] = static_cast<int>(_n[1]);
				fractalArray[y_offset + x + 3] = static_cast<int>(_n[0]);
#elif defined (_MSC_VER)
				FractalArray[y_offset + x + 0] = static_cast<int>(_n.m256i_i64[3]);
				FractalArray[y_offset + x + 1] = static_cast<int>(_n.m256i_i64[2]);
				FractalArray[y_offset + x + 2] = static_cast<int>(_n.m256i_i64[1]);
				FractalArray[y_offset + x + 3] = static_cast<int>(_n.m256i_i64[0]);
#endif

				_x_pos = SIMD_Add(_x_pos, _x_jump);
			}


			y_pos += yScale;
			y_offset += row_size;
		}
		++(*WorkerComplete);
	}
}
}
