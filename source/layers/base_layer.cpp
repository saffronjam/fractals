#include <memory>
#include "layers/base_layer.h"

namespace fractals
{
using namespace saffron;
BaseLayer::BaseLayer() :
	LayerImpl("BaseLayer"),
	_controllableRenderTexture(100, 100, sf::ContextSettings()),
	_scene("Scene", &_controllableRenderTexture, &_camera)
{
}

void BaseLayer::OnAttach(std::shared_ptr<Batch>& loader)
{
	_scene.ViewportPane().Resized.Subscribe(SE_EV_ACTION(BaseLayer::OnWantRenderTargetResize));
	RenderTargetManager::Add(&_controllableRenderTexture);

	RenderTargetResized.Subscribe([this](const sf::Vector2f& newSize)
	{
		OnRenderTargetResize(newSize);
		return false;
	});
}

void BaseLayer::OnDetach()
{
}

void BaseLayer::OnPreFrame()
{
	_dockSpace.Begin();
}

void BaseLayer::OnPostFrame()
{
	_dockSpace.End();
}

void BaseLayer::OnUpdate()
{
	if (_wantResize)
	{
		if (_framesWithNoResizeRequest > 4)
		{
			RenderTargetResized.Invoke(_resizeTo);
			_wantResize = false;
		}
		else
		{
			_framesWithNoResizeRequest++;
		}
	}

	_scene.OnUpdate();
}

void BaseLayer::OnGuiRender()
{
	Gui::Instance().OnGuiRender();
	_camera.OnGuiRender();
	_terminal.OnGuiRender();
	App::Instance().OnGuiRender();
	_scene.OnGuiRender();
}

void BaseLayer::OnRenderTargetResize(const sf::Vector2f& newSize)
{
	const auto result = _controllableRenderTexture.TryCreate(newSize.x, newSize.y, sf::ContextSettings());
	if (!result)
	{
		Log::CoreError(result.error().message);
		return;
	}

	_camera.SetViewportSize(newSize);
}

void BaseLayer::OnWantRenderTargetResize(const sf::Vector2f& newSize)
{
	if (newSize == _resizeTo)
	{
		return;
	}
	_wantResize = true;
	_resizeTo = newSize;
	_framesWithNoResizeRequest = 0;
}
}
