#include "ProjectLayer.h"

namespace Se
{
void ProjectLayer::OnAttach(std::shared_ptr<BatchLoader> &loader)
{
	BaseLayer::OnAttach(loader);

	_fractalManager = CreateShared<FractalManager>(_scene.ViewportPane().ViewportSize());

	_camera.ApplyZoom(200.0f);
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
	
	if ( ImGui::Begin("Project") )
	{
		_fractalManager->OnGuiRender();
	}
	ImGui::End();
}

void ProjectLayer::OnRenderTargetResize(const sf::Vector2f &newSize)
{
	BaseLayer::OnRenderTargetResize(newSize);
	_fractalManager->ResizeVertexArrays(newSize);
	_scene.OnRenderTargetResize(newSize);
}
}
