#include "Host.h"

namespace Se
{
Host::Host(String name, int simWidth, int simHeight) :
	_simBox(Position(), Position()),
	_name(Move(name)),
	_simWidth(simWidth),
	_simHeight(simHeight)
{
}

void Host::OnUpdate(Scene& scene)
{
	if (_computationRequested)
	{
		if (_resizeRequsted)
		{
			Resize(_desiredSize.x, _desiredSize.y);
			_simWidth = _desiredSize.x;
			_simHeight = _desiredSize.y;
			_resizeRequsted = false;
		}

		ComputeImage();
		_computationRequested = false;
	}

	if (_renderRequested)
	{
		RenderImage();
		_renderRequested = false;
	}
}

void Host::OnViewportResize(const sf::Vector2f& size)
{
	RequestResize(size);
}

auto Host::Name() const -> const String&
{
	return _name;
}

void Host::RequestImageComputation()
{
	_computationRequested = true;
}

void Host::RequestImageRendering()
{
	_renderRequested = true;
}

void Host::RequestResize(const sf::Vector2f& desiredSize)
{
	_desiredSize = desiredSize;
	_resizeRequsted = true;
}

void Host::SetComputeIterations(ulong computeIterations)
{
	_computeIterations = computeIterations;
}

auto Host::SimBox() const -> const struct SimBox&
{
	return _simBox;
}

void Host::SetSimBox(const Se::SimBox& simBox)
{
	_simBox = simBox;
}

auto Host::ComputationRequested() const -> bool
{
	return _computationRequested;
}

auto Host::RenderRequested() const -> bool
{
	return _renderRequested;
}

auto Host::ResizeRequsted() const -> bool
{
	return _resizeRequsted;
}

auto Host::ComputeIterations() const -> ulong
{
	return _computeIterations;
}

auto Host::SimWidth() const -> int
{
	return _simWidth;
}

auto Host::SimHeight() const -> int
{
	return _simHeight;
}
}
