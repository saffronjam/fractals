#pragma once

#include <complex>

#include "Fractalsets/Mandelbrot.h"
#include "Fractalsets/Julia.h"

namespace Se
{
enum class FractalGenerationPrecision
{
	Bit32,
	Bit64
};

class FractalManager
{
public:
	explicit FractalManager(const sf::Vector2f& renderSize);

	void OnUpdate(Scene& scene);
	void OnRender(Scene& scene);
	void OnGuiRender();
	void OnViewportResize(const sf::Vector2f& size);

	void ResizeVertexArrays(const sf::Vector2f& size);

private:
	void SetFractalSet(FractalSetType type);
	void SetComputeIterationCount(size_t iterations);
	void SetComputeHost(FractalSetComputeHost computeHost);
	void SetJuliaC(const Complex<double>& c);
	void SetJuliaCR(double r);
	void SetJuliaCI(double i);
	void SetPalette(FractalSetPalette palette);
	void SetMandelbrotState(Mandelbrot::State state);
	void SetJuliaState(Julia::State state);
	void SetAxisState(bool state);
	void SetPrecision(FractalGenerationPrecision precision);

	void UpdateHighPrecCamera();
	void UpdateTransform();

	auto GenerateSimBox(const Camera& camera) -> FractalSet::SimBox;

private:
	List<Unique<FractalSet>> _fractalSets;
	FractalSetType _activeFractalSet;

	FractalSet::SimBox _lastViewport;

	sf::Vector2f _viewportMousePosition = VecUtils::Null<>();

	//// Gui cache ////
	List<const char*> _fractalSetComboBoxNames;
	List<const char*> _paletteComboBoxNames;
	List<const char*> _computeHostComboBoxNames;
	List<const char*> _precisionComboBoxNames;
	int _activeFractalSetInt = static_cast<int>(FractalSetType::Mandelbrot);
	int _activePaletteInt = static_cast<int>(FractalSetPalette::Fiery);
	int _computeHostInt = -1;
	int _activePrecisionInt = static_cast<int>(FractalGenerationPrecision::Bit64);
	int _computeIterations = 64;

	// Mandelbrot
	bool _complexLines = false;

	// Julia
	int _juliaStateInt = static_cast<int>(Julia::State::None);
	sf::Vector2f _juliaC;

	// Shared
	bool _axis = false;

	// Precision
	FractalGenerationPrecision _precision = FractalGenerationPrecision::Bit64;

	// High precision transform
	FractalSet::Position _cameraPosition;
	FractalSet::Position _cameraZoom = {1.0, 1.0};
	Transform<double> _cameraZoomTransform;
	Transform<double> _cameraPositionTransform;
	Transform<double> _cameraTransform;
	FractalSet::Position _viewportSize;
};
}
