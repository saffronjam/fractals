#pragma once

#include <thread>

#include <SFML/Graphics/Image.hpp>

#include <Saffron.h>

#include "Common.h"
#include "Host.h"

namespace Se
{
enum class FractalSetType
{
	Mandelbrot,
	Julia,
	Buddhabrot
};

enum class FractalSetGenerationType
{
	AutomaticGeneration,
	DelayedGeneration,
	ManualGeneration
};

struct FractalSetPlace
{
	String Name;
	Position Position;
	double Zoom;
};

class FractalSet
{
public:
	FractalSet(String name, FractalSetType type, const sf::Vector2f& renderSize);
	virtual ~FractalSet() = default;

	virtual void OnUpdate(Scene& scene);
	virtual void OnRender(Scene& scene);
	virtual void OnViewportResize(const sf::Vector2f& size);

	void MarkForImageComputation() noexcept;
	void MarkForImageRendering() noexcept;

	void AddHost(enum class HostType type, Unique<Host> host);

	auto Name() const noexcept -> const String&;
	auto Type() const -> FractalSetType;
	auto Places() const -> const List<FractalSetPlace>&;

	auto Hosts() const -> const HashMap<enum class HostType, Unique<Host>>&;
	auto ActiveHostType() const -> HostType;
	void SetHostType(enum class HostType computeHost);

	void Resize(const sf::Vector2f& size);

	void SetSimBox(const SimBox& simBox);
	void SetComputeIterationCount(ulong iterations) noexcept;

	auto GenerationType() const -> FractalSetGenerationType;
	void SetGenerationType(FractalSetGenerationType type);

	void ActivateAxis();
	void DeactivateAxis();

	template <class FractalSetType>
	auto As() -> FractalSetType&
	{
		return dynamic_cast<FractalSetType&>(*this);
	}

	template <class FractalSetType>
	auto As() const -> const FractalSetType&
	{
		return const_cast<FractalSet&>(*this).As<FractalSetType>();
	}

	auto ActiveHost() -> Host&;
	auto ActiveHost() const -> const Host&;

protected:
	String _name;
	FractalSetType _type;
	List<FractalSetPlace> _places;
	HashMap<enum class HostType, Unique<Host>> _hosts;

	enum class HostType _activeHost = HostType::Cpu;
	FractalSetGenerationType _generationType = FractalSetGenerationType::AutomaticGeneration;

	ulong _computeIterations = 64;
	int _simWidth = 0, _simHeight = 0;
	SimBox _simBox;

private:
	// For delayed image generation
	sf::Time _lastGenerationRequest;

	// Axis
	bool _drawAxis = false;
	sf::VertexArray _axisVA;
};
}
