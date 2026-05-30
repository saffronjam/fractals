#pragma once

#include <memory>

#include "layers/base_layer.h"

#include "fractal_manager.h"
#include "palette_manager.h"

namespace fractals
{
using namespace saffron;
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
	bool _ready = false;

};
}
