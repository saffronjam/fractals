#include "Julia.h"

#include <Saffron/Core/SIMD.h>

namespace Se
{
Julia::Julia(const sf::Vector2f& renderSize) :
	FractalSet("Julia", FractalSetType::Julia, renderSize)
{
	const auto x = renderSize.x, y = renderSize.y;

	auto cpuHost = std::make_unique<CpuHost>(x, y);
	auto comHost = std::make_unique<ComputeShaderHost>("julia.comp", x, y, sf::Vector2u(x, y));
	auto pixHost = std::make_unique<PixelShaderHost>("julia.frag", x, y);

	for (int i = 0; i < 32; i++)
	{
		cpuHost->AddWorker(std::make_unique<JuliaWorker>());
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
}

void Julia::OnUpdate(Scene& scene)
{
	switch (_state)
	{
	case JuliaState::Animate:
	{
		if (_animPaused)
		{
			break;
		}

		const double x = 0.7885 * std::cos(_animationTimer);
		const double y = 0.7885 * std::sin(_animationTimer);
		SetC(std::complex(x, y), false);
		_animationTimer += Global::Clock::FrameTime().asSeconds() / 2.0f;
		if (_animationTimer > 2.0f * PI<>)
		{
			_animationTimer = 0.0f;
		}
		break;
	}
	case JuliaState::FollowCursor:
	{
		if (scene.ViewportPane().Hovered() && !Keyboard::IsDown(sf::Keyboard::Key::LControl))
		{
			const auto mousePos = scene.Camera().ScreenToWorld(scene.ViewportPane().MousePosition());
			SetC(std::complex<double>(mousePos.x, mousePos.y), false);
		}
		break;
	}
	default: break;
	}

	if (_currentC != _desiredC)
	{
		RequestImageComputation();
		RequestImageRendering();
	}
	if (_cTransitionTimer <= _cTransitionDuration && _state == JuliaState::None)
	{
		const float delta = (std::sin((_cTransitionTimer / _cTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) / 2.0f;
		_currentC.real(_startC.real() + static_cast<double>(delta) * (_desiredC.real() - _startC.real()));
		_currentC.imag(_startC.imag() + static_cast<double>(delta) * (_desiredC.imag() - _startC.imag()));
		_cTransitionTimer += Global::Clock::FrameTime().asSeconds();
	}
	else if (_state != JuliaState::None)
	{
		_currentC = _desiredC;
	}

	if (_activeHost == HostType::Cpu)
	{
		for (auto& worker : ActiveHost().As<CpuHost>().Workers())
		{
			auto& juliaWorker = dynamic_cast<JuliaWorker&>(*worker);
			juliaWorker.C = _currentC;
		}
	}

	FractalSet::OnUpdate(scene);
}

void Julia::OnRender(Scene& scene)
{
	FractalSet::OnRender(scene);

	if (_drawFlags & JuliaDrawFlags_Dot)
	{
		sf::CircleShape circle;
		const float adjustedRadius = 10.0f / scene.Camera().Zoom();
		const float adjustedThickness = 3.0f / scene.Camera().Zoom();
		const sf::Vector2f position(_currentC.real(), _currentC.imag());

		circle.setPosition(position - sf::Vector2f(adjustedRadius, adjustedRadius));
		circle.setFillColor(sf::Color(200, 50, 50));
		circle.setOutlineColor(sf::Color(100, 100, 100));
		circle.setOutlineThickness(adjustedThickness);
		circle.setRadius(adjustedRadius);
		scene.Submit(circle);
	}

	if (_drawFlags & JuliaDrawFlags_ComplexLines)
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

void Julia::OnViewportResize(const sf::Vector2f& size)
{
	FractalSet::OnViewportResize(size);
	const auto sizeU = VecUtils::ConvertTo<sf::Vector2u>(size);
	_hosts.at(HostType::GpuComputeShader)->As<ComputeShaderHost>().SetDimensions(sizeU);
}

void Julia::ResumeAnimation()
{
	_animPaused = false;
}

void Julia::PauseAnimation()
{
	_animPaused = true;
}

auto Julia::Paused() const -> bool
{
	return _animPaused;
}

auto Julia::C() const noexcept -> const std::complex<double>&
{
	return _desiredC;
}

JuliaDrawFlags Julia::DrawFlags() const
{
	return _drawFlags;
}

void Julia::SetState(JuliaState state) noexcept
{
	_state = state;
}

void Julia::SetDrawFlags(JuliaDrawFlags flags)
{
	_drawFlags = flags;
}

void Julia::SetCr(double r, bool animate)
{
	SetC(std::complex(r, C().imag()), animate);
}

void Julia::SetCi(double i, bool animate)
{
	SetC(std::complex(C().real(), i), animate);
}

auto Julia::TranslatePoint(const sf::Vector2f& point, int iterations) -> sf::Vector2f
{
	const std::complex<double> c = _currentC;
	std::complex<double> z(point.x, point.y);

	for (int n = 0; n < iterations && abs(z) < 2.0; n++)
	{
		z = (z * z) + c;
	}

	return sf::Vector2f(z.real(), z.imag());
}

void Julia::SetC(const std::complex<double>& c, bool animate)
{
	if (animate && std::abs(c - _desiredC) > 0.1)
	{
		_startC = _currentC;
		_cTransitionTimer = 0.0f;
	}
	else
	{
		RequestImageComputation();
		RequestImageRendering();
	}
	_currentC = c;
	_desiredC = c;
}

void Julia::UpdateComputeShaderUniforms(ComputeShader& shader)
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);
	shader.SetVector2d("juliaC", _currentC);
	shader.SetVector2d("fractalTL", _simBox.TopLeft);
	shader.SetDouble("xScale", xScale);
	shader.SetDouble("yScale", yScale);
	shader.SetInt("iterations", _computeIterations);
}

void Julia::UpdatePixelShaderUniforms(sf::Shader& shader)
{
	const double xScale = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(_simWidth);
	const double yScale = (_simBox.BottomRight.y - _simBox.TopLeft.y) / static_cast<double>(_simHeight);

	SetUniform(shader.getNativeHandle(), "juliaC", sf::Vector2(_currentC.real(), _currentC.imag()));
	SetUniform(shader.getNativeHandle(), "fractalTL", _simBox.TopLeft);
	SetUniform(shader.getNativeHandle(), "xScale", xScale);
	SetUniform(shader.getNativeHandle(), "yScale", yScale);
	SetUniform(shader.getNativeHandle(), "iterations", static_cast<int>(_computeIterations));
}

void Julia::JuliaWorker::Compute()
{
	while (Alive)
	{
		std::unique_lock lm(Mutex);
		CvStart.wait(lm);
		if (!Alive)
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
		_iterations = SIMD_SetOnei(Iterations);

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
