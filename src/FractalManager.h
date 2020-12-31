#pragma once

#include <string>
#include <map>
#include <mutex>

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
	void SetJuliaC(const std::complex<double> &c);
	void SetJuliaCR(double r);
	void SetJuliaCI(double i);
	void SetPalette(FractalSet::Palette palette);
	void SetMandelbrotState(Mandelbrot::State state);
	void SetJuliaState(Julia::State state);

private:
	ArrayList<std::unique_ptr<FractalSet>> _fractalSets;
	FractalSet::Type _activeFractalSet;

	Pair<sf::Vector2f, sf::Vector2f> _lastViewport;

	sf::Vector2f _viewportMousePosition = vl::Null<>();

	//// Gui cache ////
	ArrayList<const char *> _fractalSetComboBoxNames;
	ArrayList<const char *> _paletteComboBoxNames;
	int _activeFractalSetInt = static_cast<int>(FractalSet::Type::Mandelbrot);
	int _activePaletteInt = static_cast<int>(FractalSet::Palette::Fiery);
	int _computeIterations = 64;

	// Mandelbrot
	bool _complexLines = false;

	// Julia
	int _juliaStateInt = static_cast<int>(Julia::State::None);
	sf::Vector2f _juliaC = vl::Null<>();
};
}