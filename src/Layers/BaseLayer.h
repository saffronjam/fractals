#pragma once

#include <Saffron.h>

namespace Se
{
class BaseLayer : public Layer, public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<const sf::Vector2f &> OnRenderTargetResize;
	};

public:
	BaseLayer();

	void OnAttach(std::shared_ptr<BatchLoader> &loader) override;
	void OnDetach() override;

	void OnPreFrame() override;
	void OnPostFrame() override;

	void OnUpdate() override;
	void OnGuiRender() override;

protected:
	virtual void OnRenderTargetResize(const sf::Vector2f &newSize);

private:
	void OnWantRenderTargetResize(const sf::Vector2f &newSize);

protected:
	ControllableRenderTexture _controllableRenderTexture;
	Camera _camera;
	Scene _scene;
	Terminal _terminal;
	DockSpace _dockSpace;

	bool _viewSystem = false;
	bool _viewDemo = false;

private:
	bool _wantResize = false;
	sf::Vector2f _resizeTo = VecUtils::Null<>();
	int _framesWithNoResizeRequest = 0;
};

};
