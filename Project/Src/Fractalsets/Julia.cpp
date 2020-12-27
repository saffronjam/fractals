#include "Julia.h"

#include <Saffron/Core/SIMD.h>

namespace Se
{
Julia::Julia(const sf::Vector2f &renderSize)
	: FractalSet("Julia", Type::Julia, renderSize),
	_state(State::None),
	_desiredC(0.0, 0.0),
	_currentC(0.0, 0.0),
	_startC(0.0, 0.0),
	_animationTimer(0.0f),
	_cTransitionTimer(0.0f),
	_cTransitionDuration(0.5f)
{
	for ( int i = 0; i < 32; i++ )
	{
		AddWorker(new JuliaWorker);
	}
}

void Julia::OnUpdate(Scene &scene)
{
	switch ( _state )
	{
	case State::Animate:
	{
		const double x = 0.7885 * std::cos(_animationTimer);
		const double y = 0.7885 * std::sin(_animationTimer);
		SetC(std::complex<double>(x, y), false);
		_animationTimer += Global::Clock::GetFrameTime().asSeconds() / 2.0f;
		if ( _animationTimer > 2.0f * PI<> )
		{
			_animationTimer = 0.0f;
		}
		break;
	}
	case State::FollowCursor:
	{
		if ( scene.GetViewportPane().IsHovered() && !Keyboard::IsDown(sf::Keyboard::Key::LControl) )
		{
			const auto mousePos = scene.GetCamera().ScreenToWorld(scene.GetViewportPane().GetMousePosition());
			SetC(std::complex<double>(mousePos.x, mousePos.y), false);
		}
		break;
	}
	default:
		break;
	}

	if ( _currentC != _desiredC )
	{
		MarkForImageComputation();
		MarkForImageRendering();
	}
	if ( _cTransitionTimer <= _cTransitionDuration && _state == State::None )
	{
		const float delta = (std::sin((_cTransitionTimer / _cTransitionDuration) * PI<> -PI<> / 2.0f) + 1.0f) / 2.0f;
		_currentC.real(_startC.real() + static_cast<double>(delta) * (_desiredC.real() - _startC.real()));
		_currentC.imag(_startC.imag() + static_cast<double>(delta) * (_desiredC.imag() - _startC.imag()));
		_cTransitionTimer += Global::Clock::GetFrameTime().asSeconds();
	}
	else if ( _state != State::None )
	{
		_currentC = _desiredC;
	}

	for ( auto &worker : _workers )
	{
		auto *juliaWorker = dynamic_cast<JuliaWorker *>(worker);
		juliaWorker->c = _currentC;
	}

	FractalSet::OnUpdate(scene);
}

void Julia::SetC(const std::complex<double> &c, bool animate)
{
	if ( animate && abs(c - _desiredC) > 0.1f )
	{
		_startC = _currentC;
		_cTransitionTimer = 0.0f;
	}
	else
	{
		MarkForImageComputation();
		MarkForImageRendering();
	}
	_currentC = c;
	_desiredC = c;
}

void Julia::JuliaWorker::Compute()
{
	while ( alive )
	{
		std::unique_lock<std::mutex> lm(mutex);
		cvStart.wait(lm);
		if ( !alive )
		{
			nWorkerComplete++;
			return;
		}

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

		_cr = SIMD_SetOne(c.real());
		_ci = SIMD_SetOne(-c.imag()); // The negative sign is intentional

		for ( y = imageTL.y; y < imageBR.y; y++ )
		{
			// Reset x_position
			_a = SIMD_SetOne(fractalTL.x);
			_x_pos = SIMD_Add(_a, _x_pos_offsets);

			for ( x = imageTL.x; x < imageBR.x; x += 4 )
			{
				_zr = _x_pos;
				_zi = SIMD_SetOne(y_pos);
				_n = SIMD_SetZero256i();

			repeat:
				_zr2 = SIMD_Mul(_zr, _zr);
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
				_n = SIMD_Addi(_n, _c);       // n++ Increase all n
				if ( SIMD_SignMask(SIMD_CastToFloat(_mask2)) > 0 )
					goto repeat;

				fractalArray[y_offset + x + 0] = static_cast<int>(_n.m256i_i64[3]);
				fractalArray[y_offset + x + 1] = static_cast<int>(_n.m256i_i64[2]);
				fractalArray[y_offset + x + 2] = static_cast<int>(_n.m256i_i64[1]);
				fractalArray[y_offset + x + 3] = static_cast<int>(_n.m256i_i64[0]);

				_x_pos = SIMD_Add(_x_pos, _x_jump);
			}

			y_pos += yScale;
			y_offset += row_size;
		}

		++(*nWorkerComplete);
	}
}
}