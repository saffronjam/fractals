#include "Julia.h"

#include <Saffron/Core/SIMD.h>

namespace Se
{
Julia::Julia(const sf::Vector2f& renderSize) :
	FractalSet("Julia", FractalSetType::Julia, renderSize),
	_computeCS(ComputeShaderStore::Get("julia.comp")),
	_pixelShader(ShaderStore::Get("julia.frag", sf::Shader::Fragment)),
	_state(State::None),
	_desiredC(0.0, 0.0),
	_currentC(0.0, 0.0),
	_startC(0.0, 0.0),
	_animationTimer(0.0f),
	_cTransitionTimer(0.0f),
	_cTransitionDuration(0.5f)
{
	for (int i = 0; i < 32; i++)
	{
		AddWorker(new JuliaWorker);
	}
}

void Julia::OnUpdate(Scene& scene)
{
	switch (_state)
	{
	case State::Animate:
	{
		const double x = 0.7885 * std::cos(_animationTimer);
		const double y = 0.7885 * std::sin(_animationTimer);
		SetC(Complex<double>(x, y), false);
		_animationTimer += Global::Clock::FrameTime().asSeconds() / 2.0f;
		if (_animationTimer > 2.0f * PI<>)
		{
			_animationTimer = 0.0f;
		}
		break;
	}
	case State::FollowCursor:
	{
		if (scene.ViewportPane().Hovered() && !Keyboard::IsDown(sf::Keyboard::Key::LControl))
		{
			const auto mousePos = scene.Camera().ScreenToWorld(scene.ViewportPane().MousePosition());
			SetC(Complex<double>(mousePos.x, mousePos.y), false);
		}
		break;
	}
	default: break;
	}

	if (_currentC != _desiredC)
	{
		MarkForImageComputation();
		MarkForImageRendering();
	}
	if (_cTransitionTimer <= _cTransitionDuration && _state == State::None)
	{
		const float delta = (std::sin((_cTransitionTimer / _cTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) / 2.0f;
		_currentC.real(_startC.real() + static_cast<double>(delta) * (_desiredC.real() - _startC.real()));
		_currentC.imag(_startC.imag() + static_cast<double>(delta) * (_desiredC.imag() - _startC.imag()));
		_cTransitionTimer += Global::Clock::FrameTime().asSeconds();
	}
	else if (_state != State::None)
	{
		_currentC = _desiredC;
	}

	for (auto& worker : _workers)
	{
		auto* juliaWorker = dynamic_cast<JuliaWorker*>(worker);
		juliaWorker->C = _currentC;
	}

	FractalSet::OnUpdate(scene);
}


auto Julia::C() const noexcept -> const Complex<double>&
{
	return _desiredC;
}

void Julia::SetState(State state) noexcept
{
	_state = state;
}

void Julia::SetCR(double r, bool animate)
{
	SetC(Complex<double>(r, C().imag()), animate);
}

void Julia::SetCI(double i, bool animate)
{
	SetC(Complex<double>(C().real(), i), animate);
}

void Julia::SetC(const Complex<double>& c, bool animate)
{
	if (animate && abs(c - _desiredC) > 0.1)
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

auto Julia::ComputeShader() -> Shared<class ComputeShader>
{
	return _computeCS;
}

void Julia::UpdateComputeShaderUniforms()
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);
	_computeCS->SetVector2d("juliaC", _currentC);
	_computeCS->SetVector2d("fractalTL", _simBox.TopLeft);
	_computeCS->SetDouble("xScale", xScale);
	_computeCS->SetDouble("yScale", yScale);
	_computeCS->SetInt("iterations", _computeIterations);
}

auto Julia::PixelShader() -> Shared<sf::Shader>
{
	return _pixelShader;
}

void Julia::UpdatePixelShaderUniforms()
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);

	SetUniform(_pixelShader->getNativeHandle(), "juliaC", sf::Vector2<double>(_currentC.real(), _currentC.imag()));
	SetUniform(_pixelShader->getNativeHandle(), "fractalTL", _simBox.TopLeft);
	SetUniform(_pixelShader->getNativeHandle(), "xScale", xScale);
	SetUniform(_pixelShader->getNativeHandle(), "yScale", yScale);
	SetUniform(_pixelShader->getNativeHandle(), "iterations", static_cast<int>(_computeIterations));
}

void Julia::JuliaWorker::Compute()
{
	while (alive)
	{
		std::unique_lock lm(Mutex);
		CvStart.wait(lm);
		if (!alive)
		{
			WorkerComplete++;
			return;
		}

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
		_iterations = SIMD_SetOnei(iterations);

		_x_scale = SIMD_SetOne(xScale);
		_x_jump = SIMD_SetOne(xScale * 4.0);
		_x_pos_offsets = SIMD_Set(0.0, 1.0, 2.0, 3.0);
		_x_pos_offsets = SIMD_Mul(_x_pos_offsets, _x_scale);

		_cr = SIMD_SetOne(C.real());
		_ci = SIMD_SetOne(-C.imag()); // The negative sign is intentional

		for (y = ImageTL.y; y < ImageBR.y; y++)
		{
			// Reset x_position
			_a = SIMD_SetOne(FractalTL.x);
			_x_pos = SIMD_Add(_a, _x_pos_offsets);

			for (x = ImageTL.x; x < ImageBR.x; x += 4)
			{
				_zr = _x_pos;
				_zi = SIMD_SetOne(y_pos);
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
