#include "Mandelbrot.h"

#include <complex>

#include <Saffron/Core/SIMD.h>

namespace Se
{
Mandelbrot::Mandelbrot(const sf::Vector2f& renderSize) :
	FractalSet("Mandelbrot", Type::Mandelbrot, renderSize),
	_state(State::None),
	_computeCS(ComputeShaderStore::Get("mandelbrot.comp"))
{
	for (int i = 0; i < 32; i++)
	{
		AddWorker(new MandelbrotWorker);
	}
}

sf::Vector2f Mandelbrot::TranslatePoint(const sf::Vector2f& point, int iterations)
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
		scene.DeactivateScreenSpaceDrawing();
		const sf::Vector2f start = scene.GetCamera().ScreenToWorld(scene.GetViewportPane().GetMousePosition());
		sf::Vector2f to = start;
		for (int i = 1; i < _computeIterations; i++)
		{
			sf::Vector2f from = TranslatePoint(start, i);
			scene.Submit(from, to, sf::Color(200, 200, 200, 60));
			to = from;
			scene.Submit(to, sf::Color(255, 255, 255, 150), 5.0f);
		}
		scene.ActivateScreenSpaceDrawing();
	}
}

Shared<ComputeShader> Mandelbrot::GetComputeShader()
{
	return _computeCS;
}

void Mandelbrot::MandelbrotWorker::Compute()
{
	while (alive)
	{
		std::unique_lock<Mutex> lm(mutex);
		cvStart.wait(lm);
		if (!alive)
		{
			++(*nWorkerComplete);
			return;
		}
		// Somehow declaring this variable fixes a race condition
		int bugfixer = 0;

		double xScale = (fractalBR.x - fractalTL.x) / (imageBR.x - imageTL.x);
		double yScale = (fractalBR.y - fractalTL.y) / (imageBR.y - imageTL.y);

		double y_pos = fractalTL.y;

		int y_offset = 0;
		int row_size = simWidth;

		int x, y;

		SIMD_Double _a, _b, _two, _four, _mask1;
		SIMD_Double _zr, _zi, _zr2, _zi2, _cr, _ci;
		SIMD_Double _x_pos_offsets, _x_pos, _x_scale, _x_jump;
		SIMD_Integer _one, _c, _n, _iterations, _mask2;

		_one = SIMD_SetOnei(1);
		_two = SIMD_SetOne(2.0);
		_four = SIMD_SetOne(4.0);
		_iterations = SIMD_SetOnei(iterations);

		_x_scale = SIMD_SetOne(xScale);
		_x_jump = SIMD_SetOne(xScale * 4.0);
		_x_pos_offsets = SIMD_Set(0.0, 1.0, 2.0, 3.0);
		_x_pos_offsets = SIMD_Mul(_x_pos_offsets, _x_scale);

		for (y = imageTL.y; y < imageBR.y; y++)
		{
			// Reset x_position
			_a = SIMD_SetOne(fractalTL.x);
			_x_pos = SIMD_Add(_a, _x_pos_offsets);

			_ci = SIMD_SetOne(y_pos);

			for (x = imageTL.x; x < imageBR.x; x += 4)
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
				fractalArray[y_offset + x + 0] = static_cast<int>(_n.m256i_i64[3]);
				fractalArray[y_offset + x + 1] = static_cast<int>(_n.m256i_i64[2]);
				fractalArray[y_offset + x + 2] = static_cast<int>(_n.m256i_i64[1]);
				fractalArray[y_offset + x + 3] = static_cast<int>(_n.m256i_i64[0]);
#endif

				_x_pos = SIMD_Add(_x_pos, _x_jump);
			}


			y_pos += yScale;
			y_offset += row_size;
		}
		++(*nWorkerComplete);
	}
}
}
