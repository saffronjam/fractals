#pragma once

#include <Saffron.h>

#include "Common.h"

namespace Se
{
enum class HostType
{
	Cpu,
	GpuComputeShader,
	GpuPixelShader,
	Count
};

struct SimBox
{
	Position TopLeft;
	Position BottomRight;

	SimBox(const Position& topLeft, const Position& bottomRight);

	auto operator==(const SimBox& other) const -> bool;
	auto operator!=(const SimBox& other) const -> bool;
};


class Host
{
public:
	Host(HostType type, std::string name, int simWidth, int simHeight);
	virtual ~Host() = default;

	void OnUpdate(Scene& scene);
	virtual void OnRender(Scene& scene) = 0;
	virtual void OnViewportResize(const sf::Vector2f& size);

	auto Name() const -> const std::string&;
	auto Type() const -> HostType;

	auto SimBox() const -> const struct SimBox&;
	void SetSimBox(const struct SimBox& simBox);

	void RequestImageComputation();
	void RequestImageRendering();
	void RequestResize(const sf::Vector2f& desiredSize);

	void SetComputeIterations(ulong computeIterations);

	template <class HostType>
	auto As() -> HostType&
	{
		return dynamic_cast<HostType&>(*this);
	}

	template <class HostType>
	auto As() const -> const HostType&
	{
		return const_cast<Host&>(*this).As<HostType>();
	}

protected:
	auto ComputationRequested() const -> bool;
	auto RenderRequested() const -> bool;
	auto ResizeRequsted() const -> bool;
	auto ComputeIterations() const -> ulong;
	auto SimWidth() const -> int;
	auto SimHeight() const -> int;

	virtual void ComputeImage() = 0;
	virtual void RenderImage() = 0;
	virtual void Resize(int width, int height) = 0;

private:
	HostType _type;
	bool _computationRequested = true;
	bool _renderRequested = true;
	bool _resizeRequsted = true;
	std::string _name;
	sf::Vector2f _desiredSize;
	ulong _computeIterations = 64;
	struct SimBox _simBox;
	int _simWidth = 0, _simHeight = 0;
};
}
