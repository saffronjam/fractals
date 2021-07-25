#pragma once

#include "Layers/BaseLayer.h"

#include "FractalManager.h"
#include "PaletteManager.h"

namespace Se
{
class ProjectLayer : public BaseLayer
{
public:
	void OnAttach(Shared<Batch> &loader) override;
	void OnDetach() override;

	void OnUpdate() override;
	void OnGuiRender() override;

	void OnRenderTargetResize(const sf::Vector2f &newSize) override;

private:
	Unique<PaletteManager> _paletteManager;
	Shared<FractalManager> _fractalManager;

};
}
