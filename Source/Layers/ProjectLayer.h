#pragma once

#include "Layers/BaseLayer.h"

#include "FractalManager.h"
#include "PaletteManager.h"

namespace Se
{
class ProjectLayer : public BaseLayer
{
public:
	void OnAttach(std::shared_ptr<Batch> &loader) override;
	void OnDetach() override;

	void OnUpdate() override;
	void OnGuiRender() override;

	void OnRenderTargetResize(const sf::Vector2f &newSize) override;

private:
	std::unique_ptr<PaletteManager> _paletteManager;
	std::shared_ptr<FractalManager> _fractalManager;

};
}
