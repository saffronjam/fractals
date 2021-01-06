#include "FractalManager.h"

#include <Saffron.h>

namespace Se
{
FractalManager::FractalManager(const sf::Vector2f &renderSize) :
	_lastViewport(VecUtils::Null<>(), VecUtils::Null<>())
{
	_fractalSets.emplace_back(std::make_unique<Mandelbrot>(renderSize));
	_fractalSets.emplace_back(std::make_unique<Julia>(renderSize));

	_activeFractalSet = FractalSet::Type::Mandelbrot;

	for ( const auto &fractalSet : _fractalSets )
	{
		_fractalSetComboBoxNames.push_back(fractalSet->GetName().c_str());
	}

	_paletteComboBoxNames.push_back("Fiery");
	_paletteComboBoxNames.push_back("UV");
	_paletteComboBoxNames.push_back("GreyScale");
	_paletteComboBoxNames.push_back("Rainbow");
}

void FractalManager::OnUpdate(Scene &scene)
{
	if ( scene.GetViewportPane().GetViewportSize().x < 200 || scene.GetViewportPane().GetViewportSize().y < 200 )
		return;

	const auto &viewport = scene.GetCamera().GetViewport();

	if ( _lastViewport != viewport )
	{
		_lastViewport = viewport;
		_fractalSets.at(static_cast<int>(_activeFractalSet))->SetSimBox(_lastViewport);
		_fractalSets.at(static_cast<int>(_activeFractalSet))->MarkForImageComputation();
		_fractalSets.at(static_cast<int>(_activeFractalSet))->MarkForImageRendering();
	}
	_fractalSets[static_cast<int>(_activeFractalSet)]->OnUpdate(scene);
}

void FractalManager::OnRender(Scene &scene)
{
	if ( scene.GetViewportPane().GetViewportSize().x < 200 || scene.GetViewportPane().GetViewportSize().y < 200 )
		return;

	scene.ActivateScreenSpaceDrawing();
	_fractalSets.at(static_cast<int>(_activeFractalSet))->OnRender(scene);
	scene.DeactivateScreenSpaceDrawing();



	_viewportMousePosition = scene.GetViewportPane().GetMousePosition();
}

void FractalManager::OnGuiRender()
{
	Gui::BeginPropertyGrid();
	ImGui::Text("Fractal Set");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if ( ImGui::Combo("##FractalSet", &_activeFractalSetInt, _fractalSetComboBoxNames.data(), _fractalSetComboBoxNames.size()) )
	{
		SetFractalSet(static_cast<FractalSet::Type>(_activeFractalSetInt));
	}
	ImGui::NextColumn();
	ImGui::Text("Palette");
	ImGui::NextColumn();
	ImGui::PushItemWidth(-1);
	if ( ImGui::Combo("##Palette", &_activePaletteInt, _paletteComboBoxNames.data(), _paletteComboBoxNames.size()) )
	{
		SetPalette(static_cast<FractalSet::Palette>(_activePaletteInt));
	}
	ImGui::NextColumn();

	Gui::EndPropertyGrid();

	ImGui::Separator();

	Gui::BeginPropertyGrid();

	if ( Gui::Property("Iterations", _computeIterations, 10, 500, 1, Gui::PropertyFlag_Slider) )
	{
		SetComputeIterationCount(_computeIterations);
	}

	if ( _activeFractalSet == FractalSet::Type::Mandelbrot )
	{
		if ( Gui::Property("Complex lines", _complexLines) )
		{
			SetMandelbrotState(_complexLines ? Mandelbrot::State::ComplexLines : Mandelbrot::State::None);
		}
	}
	else if ( _activeFractalSet == FractalSet::Type::Julia )
	{
		const auto &julia = dynamic_cast<Julia &>(*_fractalSets[static_cast<int>(FractalSet::Type::Julia)]);
		_juliaC.x = julia.GetC().real();
		_juliaC.y = julia.GetC().imag();
		ImGui::Text("R: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if ( ImGui::SliderFloat("##R", &_juliaC.x, -6.0f, 6.0f) )
		{
			SetJuliaCR(_juliaC.x);
		}
		ImGui::NextColumn();
		ImGui::Text("I: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if ( ImGui::SliderFloat("##I", &_juliaC.y, -6.0f, 6.0f) )
		{
			SetJuliaCI(_juliaC.y);
		}
		ImGui::NextColumn();

		ImGui::Text("Mode");
		ImGui::NextColumn();
		if ( ImGui::RadioButton("None", &_juliaStateInt, static_cast<int>(Julia::State::None)) ||
			ImGui::RadioButton("Animate", &_juliaStateInt, static_cast<int>(Julia::State::Animate)) ||
			ImGui::RadioButton("Follow Cursor", &_juliaStateInt, static_cast<int>(Julia::State::FollowCursor)) )
		{
			SetJuliaState(static_cast<Julia::State>(_juliaStateInt));
		}
		ImGui::NextColumn();
	}
	Gui::EndPropertyGrid();
}

void FractalManager::ResizeVertexArrays(const sf::Vector2f &size)
{
	for ( const auto &set : _fractalSets )
	{
		set->ResizeVertexArray(size);
	}
}

void FractalManager::SetFractalSet(FractalSet::Type type)
{
	_activeFractalSet = type;
	_fractalSets.at(static_cast<int>(_activeFractalSet))->MarkForImageComputation();
	_fractalSets.at(static_cast<int>(_activeFractalSet))->MarkForImageRendering();
	// Nullify the viewport cache to force a new viewport to be computed
	_lastViewport = { VecUtils::Null<>(), VecUtils::Null<>() };
}

void FractalManager::SetComputeIterationCount(size_t iterations)
{
	for ( const auto &fractalSet : _fractalSets )
	{
		fractalSet->SetComputeIterationCount(iterations);
		fractalSet->MarkForImageComputation();
		fractalSet->MarkForImageRendering();
	}
}

void FractalManager::SetJuliaC(const std::complex<double> &c)
{
	auto &juliaSet = dynamic_cast<Julia &>(*_fractalSets[static_cast<int>(FractalSet::Type::Julia)]);
	juliaSet.SetC(c, true);
	juliaSet.MarkForImageRendering();

}

void FractalManager::SetJuliaCR(double r)
{
	auto &juliaSet = dynamic_cast<Julia &>(*_fractalSets[static_cast<int>(FractalSet::Type::Julia)]);
	juliaSet.SetCR(r, false);
	juliaSet.MarkForImageRendering();

}

void FractalManager::SetJuliaCI(double i)
{
	auto &juliaSet = dynamic_cast<Julia &>(*_fractalSets[static_cast<int>(FractalSet::Type::Julia)]);
	juliaSet.SetCI(i, false);
	juliaSet.MarkForImageRendering();
}

void FractalManager::SetPalette(FractalSet::Palette palette)
{
	for ( auto &fractalSet : _fractalSets )
	{
		fractalSet->SetPalette(palette);
		fractalSet->MarkForImageRendering();
	}
}

void FractalManager::SetMandelbrotState(Mandelbrot::State state)
{
	auto &mandelbrotSet = dynamic_cast<Mandelbrot &>(*_fractalSets[static_cast<int>(FractalSet::Type::Mandelbrot)]);
	mandelbrotSet.SetState(state);
}

void FractalManager::SetJuliaState(Julia::State state)
{
	auto &juliaSet = dynamic_cast<Julia &>(*_fractalSets[static_cast<int>(FractalSet::Type::Julia)]);
	juliaSet.SetState(state);
}
}
