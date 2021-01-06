#include "Layers/BaseLayer.h"

namespace Se
{

SignalAggregate<const sf::Vector2f &> BaseLayer::Signals::OnRenderTargetResize;

BaseLayer::BaseLayer() :
	_controllableRenderTexture(100, 100),
	_scene("Scene", &_controllableRenderTexture, &_camera)
{
}

void BaseLayer::OnAttach(std::shared_ptr<BatchLoader> &loader)
{
	_scene.GetViewportPane().GetSignal(ViewportPane::Signals::OnWantRenderTargetResize).Connect(
		[this](const sf::Vector2f &size)
		{
			OnWantRenderTargetResize(size);
		}
	);
	RenderTargetManager::Add(&_controllableRenderTexture);

	GetSignal(Signals::OnRenderTargetResize).Connect(
		[this](const sf::Vector2f &size)
		{
			OnRenderTargetResize(size);
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
	if ( _wantResize )
	{
		if ( _framesWithNoResizeRequest > 4 )
		{
			GetSignals().Emit(Signals::OnRenderTargetResize, _resizeTo);
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
	if ( _viewSystem )
	{
		_camera.OnGuiRender();
		_terminal.OnGuiRender();
		Application::Get().OnGuiRender();
	}
	_scene.OnGuiRender();
}

void BaseLayer::OnRenderTargetResize(const sf::Vector2f &newSize)
{
	_controllableRenderTexture.GetRenderTexture().create(newSize.x, newSize.y);
	_camera.SetViewportSize(newSize);
}

void BaseLayer::OnWantRenderTargetResize(const sf::Vector2f &newSize)
{
	if ( newSize == _resizeTo )
	{
		return;
	}
	_wantResize = true;
	_resizeTo = newSize;
	_framesWithNoResizeRequest = 0;
}
}
