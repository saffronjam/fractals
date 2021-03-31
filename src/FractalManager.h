#pragma once

#include <complex>

#include "Fractalsets/Mandelbrot.h"
#include "Fractalsets/Julia.h"

namespace Se
{
class FractalManager
{
public:
	explicit FractalManager(const sf::Vector2f &renderSize);

	void OnUpdate(Scene &scene);
	void OnRender(Scene &scene);
	void OnGuiRender();

	void ResizeVertexArrays(const sf::Vector2f &size);

private:
	void SetFractalSet(FractalSet::Type type);
	void SetComputeIterationCount(size_t iterations);
	void SetComputeHost(FractalSet::ComputeHost computeHost);
	void SetJuliaC(const Complex<double> &c);
	void SetJuliaCR(double r);
	void SetJuliaCI(double i);
	void SetPalette(FractalSet::Palette palette);
	void SetMandelbrotState(Mandelbrot::State state);
	void SetJuliaState(Julia::State state);

private:
	ArrayList<Unique<FractalSet>> _fractalSets;
	FractalSet::Type _activeFractalSet;

	FractalSet::SimBox _lastViewport;

	sf::Vector2f _viewportMousePosition = VecUtils::Null<>();

	//// Gui cache ////
	ArrayList<const char *> _fractalSetComboBoxNames;
	ArrayList<const char *> _paletteComboBoxNames;
	ArrayList<const char *> _computeHostComboBoxNames;
	int _activeFractalSetInt = static_cast<int>(FractalSet::Type::Mandelbrot);
	int _activePaletteInt = static_cast<int>(FractalSet::Palette::Fiery);
	int _computeHostInt = -1;
	int _computeIterations = 64;

	// Mandelbrot
	bool _complexLines = false;

	// Julia
	int _juliaStateInt = static_cast<int>(Julia::State::None);
	sf::Vector2f _juliaC = VecUtils::Null<>();
};
}