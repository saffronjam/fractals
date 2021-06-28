#pragma once

#include <Saffron.h>

namespace Se
{
class BaseLayer : public Layer
{
public:
	BaseLayer();

	void OnAttach(Shared<BatchLoader> &loader) override;
	void OnDetach() override;

	void OnPreFrame() override;
	void OnPostFrame() override;

	void OnUpdate() override;
	void OnGuiRender() override;

protected:
	virtual void OnRenderTargetResize(const sf::Vector2f &newSize);

private:
	void OnWantRenderTargetResize(const sf::Vector2f &newSize);

public:
	EventSubscriberList<const sf::Vector2f &> RenderTargetResized;

protected:
	ControllableRenderTexture _controllableRenderTexture;
	Camera _camera;
	Scene _scene;
	Terminal _terminal;
	DockSpace _dockSpace;

private:
	bool _wantResize = false;
	sf::Vector2f _resizeTo = VecUtils::Null<>();
	int _framesWithNoResizeRequest = 0;
};

};
