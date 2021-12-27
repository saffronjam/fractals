#pragma once

#include "Fractalsets/Mandelbrot.h"
#include "Fractalsets/Julia.h"
#include "Fractalsets/Buddhabrot.h"

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

	void Resize(const sf::Vector2f& size);

private:
	void SetFractalSet(FractalSetType type);
	void SetComputeIterationCount(size_t iterations);
	void SetHost(HostType computeHost);
	void SetJuliaC(const std::complex<double>& c);
	void SetJuliaCr(double r, bool animate);
	void SetJuliaCi(double i, bool animate);
	void SetPalette(PaletteType palette);
	void SetGenerationType(FractalSetGenerationType type);
	void AddMandelbrotDrawFlags(MandelbrotDrawFlags flags);
	void RemoveMandelbrotDrawFlags(MandelbrotDrawFlags flags);
	void SetJuliaState(JuliaState state);
	void AddJuliaDrawFlags(JuliaDrawFlags flags);
	void RemoveJuliaDrawFlags(JuliaDrawFlags flags);
	void SetAxisState(bool state);
	void SetPrecision(FractalGenerationPrecision precision);
	void PauseJuliaAnimation();
	void ResumeJuliaAnimation();
	
	void MarkForImageComputation();
	void MarkForImageRendering();

	void UpdateHighPrecCamera();
	void UpdateTransform();

	auto GenerateSimBox(const Camera& camera) const -> SimBox;

	auto ActiveFractalSet() -> FractalSet&;
	auto ActiveFractalSet() const -> const FractalSet&;
	auto ActiveGenerationType() -> FractalSetGenerationType;

private:
	std::vector<std::unique_ptr<FractalSet>> _fractalSets;
	FractalSetType _activeFractalSetType;

	SimBox _lastViewport;

	sf::Vector2f _viewportMousePosition = VecUtils::Null<>();

	// Gui cache
	std::vector<const char*> _fractalSetComboBoxNames;
	std::vector<const char*> _paletteComboBoxNames;
	std::vector<const char*> _precisionComboBoxNames;
	std::vector<const char*> _fractalSetGenerationTypeNames;
	std::vector<const char*> _hostNamesCache;
	std::vector<HostType> _hostTypeCache;
	int _activeFractalSetInt = static_cast<int>(FractalSetType::Mandelbrot);
	int _activePaletteInt = static_cast<int>(PaletteType::Fiery);
	int _hostInt = -1;
	int _activePrecisionInt = static_cast<int>(FractalGenerationPrecision::Bit64);
	int _fractalSetGenerationTypeInt = static_cast<int>(FractalSetGenerationType::AutomaticGeneration);
	int _computeIterations = 64;
	bool _juliaDrawComplexLines = false;
	bool _mandelbrotDrawComplexLines = false;
	bool _juliaDrawCDot = false;
	ulong _zoom = 200;

	// Animate camera movement
	Position _desiredCameraPos;
	Position _startPos;
	double _positionTransitionDuration = 0.9;
	double _positionTransitionTimer = _positionTransitionDuration + 1.0;

	double _desiredZoom = 0.0;
	double _desiredZoomLater = 0.0;
	double _startZoom = 0.0;
	double _zoomTransitionDuration = 0.9;
	double _zoomTransitionTimer = _zoomTransitionDuration + 1.0;

	// Common
	bool _manualSetIterations = false;
	
	// Julia
	int _juliaStateInt = static_cast<int>(JuliaState::None);
	sf::Vector2f _juliaC;

	// Shared
	bool _axis = false;

	// Precision
	FractalGenerationPrecision _precision = FractalGenerationPrecision::Bit64;

	// High precision transform
	Position _cameraPosition;
	Position _cameraZoom = {1.0, 1.0};
	Transform<double> _cameraZoomTransform;
	Transform<double> _cameraPositionTransform;
	Transform<double> _cameraTransform;
	Position _viewportSize;
};
}
