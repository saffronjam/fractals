#include "FractalManager.h"

#include <Saffron.h>

namespace Se
{
FractalManager::FractalManager(const sf::Vector2f& renderSize) :
	_lastViewport(VecUtils::Null<double>(), VecUtils::Null<double>()),
	_paletteComboBoxNames({"Fiery", "Fiery Alt", "UV", "Greyscale", "Rainbow"}),
	_computeHostComboBoxNames({"CPU", "GPU Compute Shader", "GPU Pixel Shader"}),
	_precisionComboBoxNames({"32-bit", "64-bit"}),
	_fractalSetGenerationTypeNames({"Automatic", "Delayed", "Manual"})
{
	_fractalSets.emplace_back(CreateUnique<Mandelbrot>(renderSize));
	_fractalSets.emplace_back(CreateUnique<Julia>(renderSize));
	_fractalSets.emplace_back(CreateUnique<Buddhabrot>(renderSize));

	_activeFractalSet = FractalSetType::Mandelbrot;

	for (const auto& fractalSet : _fractalSets)
	{
		_fractalSetComboBoxNames.push_back(fractalSet->Name().c_str());
	}

	_computeHostInt = static_cast<int>(ActiveFractalSet().ComputeHost());

	const auto factor = 200.0;
	_cameraZoom *= factor;
	_cameraZoomTransform.Scale(factor, factor);
	UpdateTransform();
}


void FractalManager::OnUpdate(Scene& scene)
{
	UpdateHighPrecCamera();
	scene.Camera().SetTransform(static_cast<sf::Transform>(_cameraTransform));

	if (scene.ViewportPane().ViewportSize().x < 200 || scene.ViewportPane().ViewportSize().y < 200) return;

	const FractalSet::SimBox sbViewport = GenerateSimBox(scene.Camera());
	if (_lastViewport != sbViewport)
	{
		_lastViewport = sbViewport;
		ActiveFractalSet().SetSimBox(_lastViewport);

		if (ActiveGenerationType() != FractalSetGenerationType::ManualGeneration)
		{
			ActiveFractalSet().MarkForImageComputation();
		}
		ActiveFractalSet().MarkForImageRendering();
	}
	ActiveFractalSet().OnUpdate(scene);
}

void FractalManager::OnRender(Scene& scene)
{
	if (scene.ViewportPane().ViewportSize().x < 200 || scene.ViewportPane().ViewportSize().y < 200) return;

	ActiveFractalSet().OnRender(scene);
	_viewportMousePosition = scene.ViewportPane().MousePosition();
}

void FractalManager::OnGuiRender()
{
	Gui::BeginPropertyGrid();
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


	if (ImGui::Combo("##Host", &_computeHostInt, _computeHostComboBoxNames.data(), _computeHostComboBoxNames.size()))
	{
		SetComputeHost(static_cast<FractalSetComputeHost>(_computeHostInt));
	}

	ImGui::NextColumn();

	ImGui::Text("Precision");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo("##Precision", &_activePrecisionInt, _precisionComboBoxNames.data(),
	                 _precisionComboBoxNames.size()))
	{
		SetPrecision(static_cast<FractalGenerationPrecision>(_activePrecisionInt));
	}
	ImGui::NextColumn();

	ImGui::Text("Generation");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo("##Generation", &_activeFractalSetGenerationTypeInt, _fractalSetGenerationTypeNames.data(),
	                 _fractalSetGenerationTypeNames.size()))
	{
		SetGenerationType(static_cast<FractalSetGenerationType>(_activeFractalSetGenerationTypeInt));
	}

	switch (static_cast<FractalSetGenerationType>(_activeFractalSetGenerationTypeInt))
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
	}

	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid("Palette");
	ImGui::Text("Palette");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo("##Palette", &_activePaletteInt, _paletteComboBoxNames.data(), _paletteComboBoxNames.size()))
	{
		SetPalette(static_cast<FractalSetPalette>(_activePaletteInt));
	}
	ImGui::NextColumn();
	Gui::EndPropertyGrid();
	ImGui::Dummy({1.0f, 2.0f});
	Gui::Image(ActiveFractalSet().PaletteTexture(), sf::Vector2f(ImGui::GetContentRegionAvailWidth(), 9.0f));

	ImGui::Separator();

	Gui::BeginPropertyGrid();

	if (Gui::Property("Axis", _axis))
	{
		SetAxisState(_axis);
	}

	if (Gui::Property("Iterations", _computeIterations, 10, 2000, 1, GuiPropertyFlag_Slider | GuiPropertyFlag_Logarithmic))
	{
		SetComputeIterationCount(_computeIterations);
	}

	Gui::EndPropertyGrid();

	ImGui::Separator();

	switch (_activeFractalSet)
	{
	case FractalSetType::Mandelbrot:
	{
		Gui::BeginPropertyGrid();
		if (Gui::Property("Complex lines", _complexLines))
		{
			SetMandelbrotState(_complexLines ? Mandelbrot::State::ComplexLines : Mandelbrot::State::None);
		}
		Gui::EndPropertyGrid();
		break;
	}
	case FractalSetType::Julia:
	{
		Gui::BeginPropertyGrid();

		const auto& julia = ActiveFractalSet().As<Julia>();

		_juliaC.x = julia.C().real();
		_juliaC.y = julia.C().imag();
		ImGui::Text("R: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat("##R", &_juliaC.x, -6.0f, 6.0f))
		{
			SetJuliaCR(_juliaC.x);
		}
		ImGui::NextColumn();
		ImGui::Text("I: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat("##I", &_juliaC.y, -6.0f, 6.0f))
		{
			SetJuliaCI(_juliaC.y);
		}
		ImGui::NextColumn();

		ImGui::Text("Mode");
		ImGui::NextColumn();
		if (ImGui::RadioButton("None", &_juliaStateInt, static_cast<int>(JuliaState::None)) || ImGui::RadioButton(
			"Animate", &_juliaStateInt, static_cast<int>(JuliaState::Animate)) || ImGui::RadioButton(
			"Follow Cursor", &_juliaStateInt, static_cast<int>(JuliaState::FollowCursor)))
		{
			SetJuliaState(static_cast<JuliaState>(_juliaStateInt));
		}
		ImGui::NextColumn();

		if (Gui::Property("Draw C-dot", _juliaDrawCDot))
		{
			_juliaDrawCDot ? AddJuliaDrawFlags(JuliaDrawFlags_Dot) : RemoveJuliaDrawFlag(JuliaDrawFlags_Dot);
		}

		Gui::EndPropertyGrid();
		break;
	}
	case FractalSetType::Buddhabrot:
	{
		if (_fractalSets[static_cast<int>(FractalSetType::Buddhabrot)]->ComputeHost() !=
			FractalSetComputeHost::GPUComputeShader)
		{
			ImGui::Text("Buddhabrot can only be rendered using Compute shader!");
		}
		break;
	}
	}
}

void FractalManager::OnViewportResize(const sf::Vector2f& size)
{
	_viewportSize = VecUtils::ConvertTo<FractalSet::Position>(size);
	ResizeVertexArrays(size);
	for (auto& fractalSet : _fractalSets)
	{
		fractalSet->OnViewportResize(size);
	}
}

void FractalManager::ResizeVertexArrays(const sf::Vector2f& size)
{
	for (const auto& set : _fractalSets)
	{
		set->Resize(size);
	}
}

void FractalManager::SetFractalSet(FractalSetType type)
{
	_activeFractalSet = type;
	ActiveFractalSet().MarkForImageComputation();
	ActiveFractalSet().MarkForImageRendering();
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
			fractalSet->MarkForImageComputation();
		}
		fractalSet->MarkForImageRendering();
	}
}

void FractalManager::SetComputeHost(FractalSetComputeHost computeHost)
{
	for (const auto& fractalSet : _fractalSets)
	{
		fractalSet->SetComputeHost(computeHost);
		fractalSet->MarkForImageComputation();
		fractalSet->MarkForImageRendering();
	}
}

void FractalManager::SetJuliaC(const Complex<double>& c)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetC(c, true);
	juliaSet.MarkForImageRendering();
}

void FractalManager::SetJuliaCR(double r)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetCR(r, false);
	juliaSet.MarkForImageRendering();
}

void FractalManager::SetJuliaCI(double i)
{
	auto& juliaSet = ActiveFractalSet().As<Julia>();
	juliaSet.SetCI(i, false);
	juliaSet.MarkForImageRendering();
}

void FractalManager::SetPalette(FractalSetPalette palette)
{
	for (auto& fractalSet : _fractalSets)
	{
		fractalSet->SetPalette(palette);
		fractalSet->MarkForImageRendering();
	}
}

void FractalManager::SetGenerationType(FractalSetGenerationType type)
{
	ActiveFractalSet().SetGenerationType(type);
}

void FractalManager::SetMandelbrotState(Mandelbrot::State state)
{
	auto& mandelbrotSet = ActiveFractalSet().As<Mandelbrot>();
	mandelbrotSet.SetState(state);
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

void FractalManager::RemoveJuliaDrawFlag(JuliaDrawFlags flags)
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
	_precision = precision;
}

void FractalManager::MarkForImageComputation()
{
	ActiveFractalSet().MarkForImageComputation();
}

void FractalManager::MarkForImageRendering()
{
	ActiveFractalSet().MarkForImageRendering();
}

void FractalManager::UpdateHighPrecCamera()
{
	if (Mouse::IsDown(sf::Mouse::Button::Left) && Mouse::IsDown(sf::Mouse::Button::Right))
	{
		auto delta = VecUtils::ConvertTo<FractalSet::Position>(Mouse::Swipe());
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

auto FractalManager::GenerateSimBox(const Camera& camera) -> FractalSet::SimBox
{
	switch (_precision)
	{
	case FractalGenerationPrecision::Bit32:
	{
		const auto& [topLeft, botRight] = camera.Viewport();
		return FractalSet::SimBox{
			FractalSet::Position(topLeft.x, topLeft.y), FractalSet::Position(botRight.x, botRight.y)
		};
	}
	case FractalGenerationPrecision::Bit64:
	{
		const auto vpSize = _viewportSize;
		const sf::Rect<double> screenRect = {{0.0, 0.0}, {vpSize.x, vpSize.y}};
		const auto TL = FractalSet::Position(screenRect.left, screenRect.top);
		const auto BR = FractalSet::Position(screenRect.left + screenRect.width, screenRect.top + screenRect.height);
		const auto inv = _cameraTransform.Inverse();

		const auto [topLeft, topRight] = CreatePair(inv.TransformPoint(TL), inv.TransformPoint(BR));

		return FractalSet::SimBox{
			FractalSet::Position(topLeft.x, topLeft.y), FractalSet::Position(topRight.x, topRight.y)
		};
	}
	}

	Debug::Break("Invalid precision type");
	return {{0, 0}, {0, 0}};
}

auto FractalManager::ActiveFractalSet() -> FractalSet&
{
	return *_fractalSets[static_cast<int>(_activeFractalSet)];
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
