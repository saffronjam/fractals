#include "FractalManager.h"

#include <Saffron.h>

namespace Se
{
FractalManager::FractalManager(const sf::Vector2f& renderSize) :
	_lastViewport(VecUtils::Null<double>(), VecUtils::Null<double>()),
	_paletteComboBoxNames({"Fiery", "Fiery Alt", "UV", "Greyscale", "Rainbow"}),
	_precisionComboBoxNames({"32-bit", "64-bit"}),
	_fractalSetGenerationTypeNames({"Automatic", "Delayed", "Manual"})
{
	_fractalSets.emplace_back(std::make_unique<Mandelbrot>(renderSize));
	_fractalSets.emplace_back(std::make_unique<Julia>(renderSize));
	_fractalSets.emplace_back(std::make_unique<Buddhabrot>(renderSize));
	_fractalSets.emplace_back(std::make_unique<Polynomial>(renderSize));

	_activeFractalSetType = FractalSetType::Mandelbrot;

	for (const auto& fractalSet : _fractalSets)
	{
		_fractalSetComboBoxNames.push_back(fractalSet->Name().c_str());
	}

	_hostInt = static_cast<int>(ActiveFractalSet().ActiveHostType());

	constexpr auto factor = 200.0;
	_cameraZoom *= factor;
	_cameraZoomTransform.Scale(factor, factor);
	UpdateTransform();
}


void FractalManager::OnUpdate(Scene& scene)
{
	scene.Camera().SetTransform(static_cast<sf::Transform>(_cameraTransform));
	scene.Camera().SetZoom(_cameraZoom.x);
	scene.Camera().SetCenter(sf::Vector2f(_cameraPosition.x, _cameraPosition.y));

	if (!_manualSetIterations)
	{
		const auto iterations = std::min(2000ull, static_cast<ulong>(std::pow(_cameraZoom.x, 0.5)) + 20);
		SetComputeIterationCount(iterations);
	}

	bool autoMove = false;
	if (_zoomTransitionTimer <= _zoomTransitionDuration)
	{
		autoMove = true;
		const auto delta = (std::sin(_zoomTransitionTimer / _zoomTransitionDuration * PI<double> - PI<double> / 2.0) +
			1.0) / 2.0;

		const auto zoom = _startZoom + delta * (_desiredZoom - _startZoom);
		_cameraZoom = Position{zoom, zoom};
		_cameraZoomTransform = Transform<double>().Scale(_cameraZoom);
		_zoomTransitionTimer += Global::Clock::FrameTime().asSeconds();

		if (_zoomTransitionTimer > _zoomTransitionDuration)
		{
			_cameraZoom = Position{_desiredZoom, _desiredZoom};
		}
	}
	else if (_positionTransitionTimer <= _positionTransitionDuration)
	{
		autoMove = true;
		const auto delta = (std::sin(
			_positionTransitionTimer / _positionTransitionDuration * PI<double> - PI<double> / 2.0) + 1.0) / 2.0;

		_cameraPosition = _startPos + delta * (_desiredCameraPos - _startPos);
		_cameraPositionTransform = Transform<double>().Translate(_cameraPosition);
		_positionTransitionTimer += Global::Clock::FrameTime().asSeconds();

		if (_positionTransitionTimer > _positionTransitionDuration)
		{
			_cameraPosition = _desiredCameraPos;
			_zoomTransitionTimer = 0.0;
			_desiredZoom = _desiredZoomLater;
			_startZoom = _cameraZoom.x;
		}
	}

	if (autoMove)
	{
		UpdateTransform();
	}
	else
	{
		UpdateHighPrecCamera();
	}


	if (scene.ViewportPane().ViewportSize().x < 200 || scene.ViewportPane().ViewportSize().y < 200) return;

	const SimBox sbViewport = GenerateSimBox(scene.Camera());
	if (_lastViewport != sbViewport)
	{
		_lastViewport = sbViewport;

		for (auto& fractalSet : _fractalSets)
		{
			fractalSet->SetSimBox(_lastViewport);
		}

		if (ActiveGenerationType() != FractalSetGenerationType::ManualGeneration)
		{
			ActiveFractalSet().RequestImageComputation();
		}
		ActiveFractalSet().RequestImageRendering();
	}
	ActiveFractalSet().OnUpdate(scene);
}

void FractalManager::OnRender(Scene& scene)
{
	if (scene.ViewportPane().ViewportSize().x < 200 || scene.ViewportPane().ViewportSize().y < 200) return;

	PaletteManager::Instance().UploadTexture();

	ActiveFractalSet().OnRender(scene);
	_viewportMousePosition = scene.ViewportPane().MousePosition();
}

void FractalManager::OnGuiRender()
{
	Gui::BeginPropertyGrid("Overview");
	ImGui::Text("Fractal Set");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo("##FractalSet", &_activeFractalSetInt, _fractalSetComboBoxNames.data(),
	                 _fractalSetComboBoxNames.size()))
	{
		SetFractalSet(static_cast<FractalSetType>(_activeFractalSetInt));
	}

	ImGui::NextColumn();

	ImGui::Text("Host");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);

	_hostNamesCache.clear();
	_hostTypeCache.clear();
	const auto& hosts = ActiveFractalSet().Hosts();

	const auto activeHostType = ActiveFractalSet().ActiveHostType();
	{
		int index = 0;
		for (const auto& [type, host] : hosts)
		{
			_hostNamesCache.push_back(host->Name().c_str());
			_hostTypeCache.push_back(type);
			if (type == activeHostType)
			{
				_hostInt = index;
			}
			index++;
		}
	}

	if (ImGui::Combo("##Host", &_hostInt, _hostNamesCache.data(), _hostNamesCache.size()))
	{
		SetHost(_hostTypeCache[_hostInt]);
	}

	ImGui::NextColumn();

	ImGui::Text("Generation");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo("##Generation", &_fractalSetGenerationTypeInt, _fractalSetGenerationTypeNames.data(),
	                 _fractalSetGenerationTypeNames.size()))
	{
		SetGenerationType(static_cast<FractalSetGenerationType>(_fractalSetGenerationTypeInt));
	}

	switch (static_cast<FractalSetGenerationType>(_fractalSetGenerationTypeInt))
	{
	case FractalSetGenerationType::ManualGeneration:
	{
		if (ImGui::Button("[G]enerate", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f)) || Keyboard::IsPressed(
			sf::Keyboard::G))
		{
			MarkForImageRendering();
			MarkForImageComputation();
		}
		break;
	}
	case FractalSetGenerationType::AutomaticGeneration: break;
	case FractalSetGenerationType::DelayedGeneration: break;
	}

	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("Palette");
	ImGui::Text("Palette");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo("##Palette", &_activePaletteInt, _paletteComboBoxNames.data(), _paletteComboBoxNames.size()))
	{
		SetPalette(static_cast<PaletteType>(_activePaletteInt));
	}
	ImGui::NextColumn();
	Gui::EndPropertyGrid();
	ImGui::Dummy({1.0f, 2.0f});
	Gui::Image(PaletteManager::Instance().Texture(), sf::Vector2f(ImGui::GetContentRegionAvailWidth(), 9.0f));

	ImGui::Separator();

	Gui::BeginPropertyGrid("Common settings");

	ImGui::Text("Manual Iterations");
	ImGui::NextColumn();
	if (ImGui::Checkbox("##AutoSetIterations", &_manualSetIterations))
	{
		if (_manualSetIterations)
		{
			SetComputeIterationCount(_computeIterations);
		}
	}
	ImGui::PushItemWidth(-1);
	if (_manualSetIterations)
	{
		ImGui::SameLine();
		if (ImGui::SliderInt("##SliderIterations", &_computeIterations, 10, 2000, "%d", ImGuiSliderFlags_Logarithmic))
		{
			SetComputeIterationCount(_computeIterations);
		}
	}
	ImGui::NextColumn();

	ImGui::Text("Zoom");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	_zoom = _cameraZoom.x;
	if (ImGui::DragScalar("##Zoom", ImGuiDataType_U64, &_zoom, 7))
	{
		_cameraZoom = Position(_zoom, _zoom);
		_cameraPositionTransform = Transform<double>().Scale(_cameraZoom);
	}

	ImGui::NextColumn();

	ImGui::Text("R");
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::DragScalar("##PosR", ImGuiDataType_Double, &_cameraPosition.x,
	                      1.0 / (static_cast<double>(_zoom) / 10.0)))
	{
		_cameraPositionTransform = Transform<double>().Scale(_cameraPosition);
	}
	ImGui::NextColumn();
	ImGui::Text("I");
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::DragScalar("##PosI", ImGuiDataType_Double, &_cameraPosition.y,
	                      1.0 / (static_cast<double>(_zoom) / 10.0)))
	{
		_cameraPositionTransform = Transform<double>().Scale(_cameraPosition);
	}
	ImGui::NextColumn();

	Gui::EndPropertyGrid();

	ImGui::Separator();

	bool anyAdd = true;

	// Draw options

	Gui::BeginPropertyGrid("DrawOptions");

	if (Gui::Property("Axis", _axis))
	{
		SetAxisState(_axis);
	}

	Gui::EndPropertyGrid();

	switch (_activeFractalSetType)
	{
	case FractalSetType::Mandelbrot:
	{
		Gui::BeginPropertyGrid("DrawOptions");
		if (Gui::Property("Complex lines", _mandelbrotDrawComplexLines))
		{
			_mandelbrotDrawComplexLines
				? AddMandelbrotDrawFlags(MandelbrotDrawFlags_ComplexLines)
				: RemoveMandelbrotDrawFlags(MandelbrotDrawFlags_ComplexLines);
		}
		Gui::EndPropertyGrid();
		break;
	}
	case FractalSetType::Julia:
	{
		Gui::BeginPropertyGrid("DrawOptions");

		if (Gui::Property("Draw C-dot", _juliaDrawCDot))
		{
			_juliaDrawCDot ? AddJuliaDrawFlags(JuliaDrawFlags_Dot) : RemoveJuliaDrawFlags(JuliaDrawFlags_Dot);
		}

		if (Gui::Property("Complex lines", _juliaDrawComplexLines))
		{
			_juliaDrawComplexLines
				? AddJuliaDrawFlags(JuliaDrawFlags_ComplexLines)
				: RemoveJuliaDrawFlags(JuliaDrawFlags_ComplexLines);
		}
		Gui::EndPropertyGrid();
		break;
	}
	case FractalSetType::Buddhabrot:
	{
		break;
	}
	case FractalSetType::Polynomial:
	{
		break;
	}
	}

	if (anyAdd) ImGui::Separator();

	anyAdd = true;

	// Fractal set specific control gui

	switch (_activeFractalSetType)
	{
	case FractalSetType::Mandelbrot:
	{
		anyAdd = false;
		break;
	}
	case FractalSetType::Julia:
	{
		Gui::BeginPropertyGrid();

		const auto& julia = ActiveFractalSet().As<Julia>();

		_juliaC.x = julia.C().real();
		_juliaC.y = julia.C().imag();
		ImGui::Text("R");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		const sf::Vector2f lastJuliaC = _juliaC;
		if (ImGui::SliderFloat("##R", &_juliaC.x, -6.0f, 6.0f))
		{
			const auto animate = std::abs(_juliaC.x - lastJuliaC.x) > 1.0f;
			SetJuliaCr(_juliaC.x, animate);
		}
		ImGui::NextColumn();
		ImGui::Text("I");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat("##I", &_juliaC.y, -6.0f, 6.0f))
		{
			const auto animate = std::abs(_juliaC.y - lastJuliaC.x) > 1.0f;
			SetJuliaCi(_juliaC.y, animate);
		}
		ImGui::NextColumn();

		ImGui::Text("Mode");
		ImGui::NextColumn();


		if (ImGui::RadioButton("None", &_juliaStateInt, static_cast<int>(JuliaState::None)))
		{
			SetJuliaState(static_cast<JuliaState>(_juliaStateInt));
		}

		if (ImGui::RadioButton("Animate", &_juliaStateInt, static_cast<int>(JuliaState::Animate)))
		{
			SetJuliaState(static_cast<JuliaState>(_juliaStateInt));
		}

		if (_juliaStateInt == static_cast<int>(JuliaState::Animate))
		{
			const auto state = ActiveFractalSet().As<Julia>().Paused();
			if (ImGui::Button(state ? "Resume" : "Pause", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f)))
			{
				state ? ResumeJuliaAnimation() : PauseJuliaAnimation();
			}
		}

		if (ImGui::RadioButton("Follow Cursor", &_juliaStateInt, static_cast<int>(JuliaState::FollowCursor)))
		{
			SetJuliaState(static_cast<JuliaState>(_juliaStateInt));
		}

		ImGui::NextColumn();


		Gui::EndPropertyGrid();
		break;
	}
	case FractalSetType::Buddhabrot:
	{
		if (_fractalSets[static_cast<int>(FractalSetType::Buddhabrot)]->ActiveHostType() != HostType::GpuComputeShader)
		{
			ImGui::Text("Buddhabrot can only be rendered using Compute shader!");
		}
		else
		{
			anyAdd = false;
		}
		break;
	}
	case FractalSetType::Polynomial:
	{
		Gui::BeginPropertyGrid();

		const auto& polynomial = ActiveFractalSet().As<Polynomial>();

		_polynomialConstants = GenUtils::ConvertArrayTo<float>(polynomial.Constants());

		const auto constantNameArr = std::array{"3rd", "2nd", "1st", "Literal"};
		for (size_t i = 0; i < _polynomialConstants.size(); i++)
		{
			ImGui::Text("%s", constantNameArr[i]);
			ImGui::NextColumn();
			ImGui::PushItemWidth(-1);
			const auto lastConstant = _polynomialConstants[i];
			const auto id = std::string("##PolynomialConstants") + std::to_string(i);
			if (ImGui::SliderFloat(id.c_str(), &_polynomialConstants[i], -6.0f, 6.0f))
			{
				const auto animate = std::abs(_polynomialConstants[i] - lastConstant) > 1.0;
				SetPolynomialConstants(_polynomialConstants, false);
			}
			ImGui::NextColumn();
		}

		Gui::EndPropertyGrid();
		break;
	}
	}

	if (anyAdd) ImGui::Separator();


	// Places
	Gui::BeginPropertyGrid();

	const auto& places = ActiveFractalSet().Places();
	for (const auto& [name, position, zoom] : places)
	{
		ImGui::Text("%s", name.c_str());
		ImGui::NextColumn();
		if (ImGui::Button("Go to", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f)))
		{
			_positionTransitionTimer = 0.0;
			_desiredCameraPos = position;
			_startPos = _cameraPosition;

			_zoomTransitionTimer = 0.0;
			_desiredZoom = 200.0;
			_desiredZoomLater = zoom;
			_startZoom = _cameraZoom.x;
		}
		ImGui::NextColumn();
	}

	Gui::EndPropertyGrid();
}

void FractalManager::OnViewportResize(const sf::Vector2f& size)
{
	_viewportSize = VecUtils::ConvertTo<Position>(size);
	Resize(size);
	for (auto& fractalSet : _fractalSets)
	{
		fractalSet->OnViewportResize(size);
	}
}

void FractalManager::Resize(const sf::Vector2f& size)
{
	for (const auto& set : _fractalSets)
	{
		set->Resize(size);
	}
}

void FractalManager::SetFractalSet(FractalSetType type)
{
	_activeFractalSetType = type;

	auto& activeFractalSet = ActiveFractalSet();
	activeFractalSet.RequestImageComputation();
	activeFractalSet.RequestImageRendering();

	_fractalSetGenerationTypeInt = static_cast<int>(ActiveFractalSet().GenerationType());

	// Nullify the viewport cache to force a new viewport to be computed
	_lastViewport = {VecUtils::Null<double>(), VecUtils::Null<double>()};
}

void FractalManager::SetComputeIterationCount(size_t iterations)
{
	for (const auto& fractalSet : _fractalSets)
	{
		fractalSet->SetComputeIterationCount(iterations);
		if (ActiveGenerationType() != FractalSetGenerationType::ManualGeneration)
		{
			fractalSet->RequestImageComputation();
		}
		fractalSet->RequestImageRendering();
	}
}

void FractalManager::SetHost(HostType computeHost)
{
	ActiveFractalSet().SetHostType(computeHost);
	ActiveFractalSet().RequestImageComputation();
	ActiveFractalSet().RequestImageRendering();
}

void FractalManager::SetJuliaC(const std::complex<double>& c)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetC(c, true);
	juliaSet.RequestImageRendering();
}

void FractalManager::SetJuliaCr(double r, bool animate)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetCr(r, animate);
	juliaSet.RequestImageRendering();
}

void FractalManager::SetJuliaCi(double i, bool animate)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetCi(i, animate);
	juliaSet.RequestImageRendering();
}

void FractalManager::SetPolynomialConstants(const std::array<float, Polynomial::PolynomialDegree>& constants,
                                            bool animate)
{
	auto& polynomial = ActiveFractalSet().As<Polynomial>();
	polynomial.SetConstants(GenUtils::ConvertArrayTo<double>(constants), animate);
	polynomial.RequestImageRendering();
}

void FractalManager::SetPalette(PaletteType palette)
{
	PaletteManager::Instance().SetActive(palette);
	for (auto& fractalSet : _fractalSets)
	{
		fractalSet->RequestImageRendering();
	}
}

void FractalManager::SetGenerationType(FractalSetGenerationType type)
{
	ActiveFractalSet().SetGenerationType(type);
}

void FractalManager::AddMandelbrotDrawFlags(MandelbrotDrawFlags flags)
{
	auto& mandelbrotSet = ActiveFractalSet().As<Mandelbrot>();
	mandelbrotSet.SetDrawFlags(mandelbrotSet.DrawFlags() | flags);
}

void FractalManager::RemoveMandelbrotDrawFlags(MandelbrotDrawFlags flags)
{
	auto& mandelbrotSet = ActiveFractalSet().As<Mandelbrot>();
	mandelbrotSet.SetDrawFlags(mandelbrotSet.DrawFlags() & ~flags);
}

void FractalManager::SetJuliaState(JuliaState state)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetState(state);
}

void FractalManager::AddJuliaDrawFlags(JuliaDrawFlags flags)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetDrawFlags(juliaSet.DrawFlags() | flags);
}

void FractalManager::RemoveJuliaDrawFlags(JuliaDrawFlags flags)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetDrawFlags(juliaSet.DrawFlags() & ~flags);
}

void FractalManager::SetAxisState(bool state)
{
	for (auto& fractalSet : _fractalSets)
	{
		if (_axis)
		{
			fractalSet->ActivateAxis();
		}
		else
		{
			fractalSet->DeactivateAxis();
		}
	}
}

void FractalManager::SetPrecision(FractalGenerationPrecision precision)
{
	// It is POSSIBLE to use 32-bit floating point precision.
	// But it was removed from Gui, since it cluttered more than
	// it provided
	_precision = precision;
}

void FractalManager::PauseJuliaAnimation()
{
	ActiveFractalSet().As<Julia>().PauseAnimation();
}

void FractalManager::ResumeJuliaAnimation()
{
	ActiveFractalSet().As<Julia>().ResumeAnimation();
}

void FractalManager::MarkForImageComputation()
{
	ActiveFractalSet().RequestImageComputation();
}

void FractalManager::MarkForImageRendering()
{
	ActiveFractalSet().RequestImageRendering();
}

void FractalManager::UpdateHighPrecCamera()
{
	if (Mouse::IsDown(sf::Mouse::Button::Left) && Mouse::IsDown(sf::Mouse::Button::Right))
	{
		auto delta = VecUtils::ConvertTo<Position>(Mouse::Swipe());
		if (VecUtils::LengthSq(delta) > 0.0)
		{
			delta = _cameraZoomTransform.Inverse().TransformPoint(delta);
			delta *= -1.0;

			const auto center = _cameraPosition + delta;
			_cameraPosition = center;
			_cameraPositionTransform = Transform<double>().Translate(_cameraPosition);
			UpdateTransform();
		}
	}

	const auto factor = static_cast<double>(Mouse::VerticalScroll()) / 100.0 + 1.0;
	_cameraZoom *= factor;
	_cameraZoomTransform.Scale(factor, factor);
	UpdateTransform();

	if (Keyboard::IsPressed(sf::Keyboard::R))
	{
		_cameraPosition = {0.0, 0.0};
		_cameraPositionTransform = Transform<double>().Translate(_cameraPosition);

		_cameraZoom = {1.0, 1.0};
		_cameraZoomTransform = Transform<double>::Identity;

		const auto resetFactor = 200.0;
		_cameraZoom *= resetFactor;
		_cameraZoomTransform.Scale(resetFactor, resetFactor);
		UpdateTransform();
	}
}

void FractalManager::UpdateTransform()
{
	_cameraTransform = Transform<double>::Identity;
	_cameraTransform.Translate(_viewportSize / 2.0);
	_cameraTransform.Scale(_cameraZoom);
	_cameraTransform.Translate(-_cameraPosition);
}

auto FractalManager::GenerateSimBox(const Camera& camera) const -> SimBox
{
	switch (_precision)
	{
	case FractalGenerationPrecision::Bit32:
	{
		const auto& [topLeft, botRight] = camera.Viewport();
		return SimBox{Position(topLeft.x, topLeft.y), Position(botRight.x, botRight.y)};
	}
	case FractalGenerationPrecision::Bit64:
	{
		const auto vpSize = _viewportSize;
		const sf::Rect<double> screenRect = {{0.0, 0.0}, {vpSize.x, vpSize.y}};
		const auto TL = Position(screenRect.left, screenRect.top);
		const auto BR = Position(screenRect.left + screenRect.width, screenRect.top + screenRect.height);
		const auto inv = _cameraTransform.Inverse();

		const auto [topLeft, topRight] = std::make_pair(inv.TransformPoint(TL), inv.TransformPoint(BR));

		return SimBox{Position(topLeft.x, topLeft.y), Position(topRight.x, topRight.y)};
	}
	}

	Debug::Break("Invalid precision type");
	return {{0, 0}, {0, 0}};
}

auto FractalManager::ActiveFractalSet() -> FractalSet&
{
	return *_fractalSets[static_cast<int>(_activeFractalSetType)];
}

auto FractalManager::ActiveFractalSet() const -> const FractalSet&
{
	return const_cast<FractalManager&>(*this).ActiveFractalSet();
}

auto FractalManager::ActiveGenerationType() -> FractalSetGenerationType
{
	return ActiveFractalSet().GenerationType();
}
}
