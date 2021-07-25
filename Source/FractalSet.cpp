#include "FractalSet.h"

#include <glad/glad.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "PaletteManager.h"

namespace Se
{
SimBox::SimBox(const Position& topLeft, const Position& bottomRight) :
	TopLeft(topLeft),
	BottomRight(bottomRight)
{
}

auto SimBox::operator==(const SimBox& other) const -> bool
{
	return TopLeft == other.TopLeft && BottomRight == other.BottomRight;
}

auto SimBox::operator!=(const SimBox& other) const -> bool
{
	return !(*this == other);
}


FractalSet::FractalSet(String name, FractalSetType type, const sf::Vector2f& renderSize) :
	_name(Move(name)),
	_type(type),
	_simWidth(renderSize.x),
	_simHeight(renderSize.y),
	_simBox(Position(), Position()),
	_axisVA(sf::PrimitiveType::Lines)
{
	PaletteManager::Instance().PaletteUpdated += [this]
	{
		MarkForImageRendering();
		return false;
	};

	// Setup Axis VA
	_axisVA.append(sf::Vertex(sf::Vector2f(-3.0f, 0.0f), sf::Color::White));
	_axisVA.append(sf::Vertex(sf::Vector2f(3.0f, 0.0f), sf::Color::White));
	_axisVA.append(sf::Vertex(sf::Vector2f(0.0f, -3.0f), sf::Color::White));
	_axisVA.append(sf::Vertex(sf::Vector2f(0.0f, 3.0f), sf::Color::White));

	constexpr float lineOffset1 = 0.1f;
	constexpr float lineOffset10 = 0.01f;
	constexpr float lineOffset100 = 0.001f;
	constexpr float lineOffset1000 = 0.0001f;

	const int range = 3000;
	for (int i = -range; i <= range; i++)
	{
		float offset = lineOffset1000;
		if (i % 1000 == 0)
		{
			offset = lineOffset1;
		}
		else if (i % 100 == 0)
		{
			offset = lineOffset10;
		}
		else if (i % 10 == 0)
		{
			offset = lineOffset100;
		}

		_axisVA.append(sf::Vertex(sf::Vector2f(static_cast<float>(i) / 1000.0f, -offset), sf::Color::White));
		_axisVA.append(sf::Vertex(sf::Vector2f(static_cast<float>(i) / 1000.0f, offset), sf::Color::White));
		_axisVA.append(sf::Vertex(sf::Vector2f(-offset, static_cast<float>(i) / 1000.0f), sf::Color::White));
		_axisVA.append(sf::Vertex(sf::Vector2f(offset, static_cast<float>(i) / 1000.0f), sf::Color::White));
	}

	PaletteManager::Instance().PaletteUpdated += [this]()
	{
		MarkForImageRendering();
		return false;
	};
}

void FractalSet::OnUpdate(Scene& scene)
{
	const auto start = Global::Clock::SinceStart();

	ActiveHost().OnUpdate(scene);
	if (_generationType == FractalSetGenerationType::DelayedGeneration)
	{
		if (start - _lastGenerationRequest > sf::seconds(0.2f))
		{
			_lastGenerationRequest = start;
		}
	}
}

void FractalSet::OnRender(Scene& scene)
{
	ActiveHost().OnRender(scene);
	if (_drawAxis)
	{
		scene.Submit(_axisVA);
	}
}

void FractalSet::OnViewportResize(const sf::Vector2f& size)
{
	for (auto& host : _hosts | std::views::values)
	{
		host->OnViewportResize(size);
	}
}

void FractalSet::MarkForImageRendering() noexcept
{
	ActiveHost().RequestImageRendering();
}

void FractalSet::AddHost(enum class HostType type, Unique<Host> host)
{
	_hosts.emplace(type, Move(host));
}

void FractalSet::MarkForImageComputation() noexcept
{
	_lastGenerationRequest = Global::Clock::SinceStart();
	ActiveHost().RequestImageComputation();
}

auto FractalSet::Name() const noexcept -> const String&
{
	return _name;
}

auto FractalSet::Type() const -> FractalSetType
{
	return _type;
}

auto FractalSet::Places() const -> const List<FractalSetPlace>&
{
	return _places;
}

auto FractalSet::Hosts() const -> const HashMap<enum class HostType, Unique<Host>>&
{
	return _hosts;
}

auto FractalSet::ActiveHostType() const -> enum class HostType
{
	return _activeHost;
}

void FractalSet::SetHostType(enum class HostType computeHost)
{
	Debug::Assert(_hosts.contains(computeHost));
	_activeHost = computeHost;
}

void FractalSet::Resize(const sf::Vector2f& size)
{
	_simWidth = size.x;
	_simHeight = size.y;
	ActiveHost().RequestResize(size);
}

void FractalSet::SetSimBox(const SimBox& simBox)
{
	_simBox = simBox;
	ActiveHost().SetSimBox(simBox);
}

void FractalSet::SetComputeIterationCount(ulong iterations) noexcept
{
	_computeIterations = iterations;
	for (auto& host : _hosts | std::views::values)
	{
		host->SetComputeIterations(iterations);
	}
}

auto FractalSet::GenerationType() const -> FractalSetGenerationType
{
	return _generationType;
}

void FractalSet::SetGenerationType(FractalSetGenerationType type)
{
	_generationType = type;
	MarkForImageComputation();
	MarkForImageRendering();
}

void FractalSet::ActivateAxis()
{
	_drawAxis = true;
}

void FractalSet::DeactivateAxis()
{
	_drawAxis = false;
}

auto FractalSet::ActiveHost() -> Host&
{
	Debug::Assert(_hosts.contains(_activeHost));
	return *_hosts.at(_activeHost);
}

auto FractalSet::ActiveHost() const -> const Host&
{
	return const_cast<FractalSet&>(*this).ActiveHost();
}
}
