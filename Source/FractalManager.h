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
	void SetFractalSet(FractalSetType type);
	void SetComputeIterationCount(size_t iterations);
	void SetComputeHost(FractalSetComputeHost computeHost);
	void SetJuliaC(const Complex<double> &c);
	void SetJuliaCR(double r);
	void SetJuliaCI(double i);
	void SetPalette(FractalSetPalette palette);
	void SetMandelbrotState(Mandelbrot::State state);
	void SetJuliaState(Julia::State state);
	void SetAxisState(bool state);

private:
	List<Unique<FractalSet>> _fractalSets;
	FractalSetType _activeFractalSet;

	FractalSet::SimBox _lastViewport;

	sf::Vector2f _viewportMousePosition = VecUtils::Null<>();

	//// Gui cache ////
	List<const char *> _fractalSetComboBoxNames;
	List<const char *> _paletteComboBoxNames;
	List<const char *> _computeHostComboBoxNames;
	int _activeFractalSetInt = static_cast<int>(FractalSetType::Mandelbrot);
	int _activePaletteInt = static_cast<int>(FractalSetPalette::Fiery);
	int _computeHostInt = -1;
	int _computeIterations = 64;

	// Mandelbrot
	bool _complexLines = false;

	// Julia
	int _juliaStateInt = static_cast<int>(Julia::State::None);
	sf::Vector2f _juliaC = VecUtils::Null<>();

	// Shared
	bool _axis = false;
};
}