#include "ProjectLayer.h"

namespace Se
{
void ProjectLayer::OnAttach(Shared<BatchLoader>& loader)
{
	BaseLayer::OnAttach(loader);

	_fractalManager = CreateShared<FractalManager>(_scene.ViewportPane().ViewportSize());

	_camera.ApplyZoom(200.0f);
	_camera.Reset += [this]()
	{
		_camera.ApplyZoom(200.0f);
		return false;
	};
	_camera.Disable();
}

void ProjectLayer::OnDetach()
{
	BaseLayer::OnDetach();
}

void ProjectLayer::OnUpdate()
{
	BaseLayer::OnUpdate();

	_fractalManager->OnUpdate(_scene);
	_fractalManager->OnRender(_scene);
}

void ProjectLayer::OnGuiRender()
{
	BaseLayer::OnGuiRender();

	if (ImGui::Begin("Project"))
	{
		_fractalManager->OnGuiRender();
	}
	ImGui::End();
}

void ProjectLayer::OnRenderTargetResize(const sf::Vector2f& newSize)
{
	BaseLayer::OnRenderTargetResize(newSize);
	_fractalManager->OnViewportResize(newSize);
	_scene.OnRenderTargetResize(newSize);
}
}
