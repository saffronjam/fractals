#include "FractalManager.h"

#include <Saffron.h>

namespace Se
{
FractalManager::FractalManager(const sf::Vector2f& renderSize) :
	_lastViewport(VecUtils::Null<double>(), VecUtils::Null<double>()),
	_paletteComboBoxNames({"Fiery", "UV", "Greyscale", "Rainbow"}),
	_computeHostComboBoxNames({"CPU", "GPU Compute Shader", "GPU Pixel Shader"})
{
	_fractalSets.emplace_back(CreateUnique<Mandelbrot>(renderSize));
	_fractalSets.emplace_back(CreateUnique<Julia>(renderSize));

	_activeFractalSet = FractalSetType::Mandelbrot;

	for (const auto& fractalSet : _fractalSets)
	{
		_fractalSetComboBoxNames.push_back(fractalSet->Name().c_str());
	}

	_computeHostInt = static_cast<int>(_fractalSets.at(static_cast<int>(_activeFractalSet))->ComputeHost());
}

void FractalManager::OnUpdate(Scene& scene)
{
	if (scene.ViewportPane().ViewportSize().x < 200 || scene.ViewportPane().ViewportSize().y < 200) return;

	const auto& viewport = scene.Camera().Viewport();
	const FractalSet::SimBox sbViewport(FractalSet::Position(viewport.first.x, viewport.first.y),
	                                    FractalSet::Position(viewport.second.x, viewport.second.y));

	if (_lastViewport != sbViewport)
	{
		_lastViewport = sbViewport;
		_fractalSets.at(static_cast<int>(_activeFractalSet))->SetSimBox(_lastViewport);
		_fractalSets.at(static_cast<int>(_activeFractalSet))->MarkForImageComputation();
		_fractalSets.at(static_cast<int>(_activeFractalSet))->MarkForImageRendering();
	}
	_fractalSets[static_cast<int>(_activeFractalSet)]->OnUpdate(scene);
}

void FractalManager::OnRender(Scene& scene)
{
	if (scene.ViewportPane().ViewportSize().x < 200 || scene.ViewportPane().ViewportSize().y < 200) return;

	_fractalSets.at(static_cast<int>(_activeFractalSet))->OnRender(scene);
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
	Gui::Image(_fractalSets.at(static_cast<int>(_activeFractalSet))->PaletteTexture(),
	           sf::Vector2f(ImGui::GetContentRegionAvailWidth(), 9.0f));

	ImGui::Separator();

	Gui::BeginPropertyGrid();

	if (Gui::Property("Axis", _axis))
	{
		SetAxisState(_axis);
	}

	if (Gui::Property("Iterations", _computeIterations, 10, 800, 1, GuiPropertyFlag_Slider))
	{
		SetComputeIterationCount(_computeIterations);
	}

	if (_activeFractalSet == FractalSetType::Mandelbrot)
	{
		if (Gui::Property("Complex lines", _complexLines))
		{
			SetMandelbrotState(_complexLines ? Mandelbrot::State::ComplexLines : Mandelbrot::State::None);
		}
	}
	else if (_activeFractalSet == FractalSetType::Julia)
	{
		const auto& julia = dynamic_cast<Julia&>(*_fractalSets[static_cast<int>(FractalSetType::Julia)]);
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
		if (ImGui::RadioButton("None", &_juliaStateInt, static_cast<int>(Julia::State::None)) || ImGui::RadioButton(
			"Animate", &_juliaStateInt, static_cast<int>(Julia::State::Animate)) || ImGui::RadioButton(
			"Follow Cursor", &_juliaStateInt, static_cast<int>(Julia::State::FollowCursor)))
		{
			SetJuliaState(static_cast<Julia::State>(_juliaStateInt));
		}
		ImGui::NextColumn();
	}
	Gui::EndPropertyGrid();
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
	_fractalSets.at(static_cast<int>(_activeFractalSet))->MarkForImageComputation();
	_fractalSets.at(static_cast<int>(_activeFractalSet))->MarkForImageRendering();
	// Nullify the viewport cache to force a new viewport to be computed
	_lastViewport = {VecUtils::Null<double>(), VecUtils::Null<double>()};
}

void FractalManager::SetComputeIterationCount(size_t iterations)
{
	for (const auto& fractalSet : _fractalSets)
	{
		fractalSet->SetComputeIterationCount(iterations);
		fractalSet->MarkForImageComputation();
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
	auto& juliaSet = dynamic_cast<Julia&>(*_fractalSets[static_cast<int>(FractalSetType::Julia)]);
	juliaSet.SetC(c, true);
	juliaSet.MarkForImageRendering();
}

void FractalManager::SetJuliaCR(double r)
{
	auto& juliaSet = dynamic_cast<Julia&>(*_fractalSets[static_cast<int>(FractalSetType::Julia)]);
	juliaSet.SetCR(r, false);
	juliaSet.MarkForImageRendering();
}

void FractalManager::SetJuliaCI(double i)
{
	auto& juliaSet = dynamic_cast<Julia&>(*_fractalSets[static_cast<int>(FractalSetType::Julia)]);
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

void FractalManager::SetMandelbrotState(Mandelbrot::State state)
{
	auto& mandelbrotSet = dynamic_cast<Mandelbrot&>(*_fractalSets[static_cast<int>(FractalSetType::Mandelbrot)]);
	mandelbrotSet.SetState(state);
}

void FractalManager::SetJuliaState(Julia::State state)
{
	auto& juliaSet = dynamic_cast<Julia&>(*_fractalSets[static_cast<int>(FractalSetType::Julia)]);
	juliaSet.SetState(state);
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
}
