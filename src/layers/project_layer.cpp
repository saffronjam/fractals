#include "project_layer.h"
#include <memory>

namespace fractals {
using namespace saffron;
void ProjectLayer::OnAttach(std::shared_ptr<Batch>& loader) {
    BaseLayer::OnAttach(loader);

    _paletteManager = std::make_unique<PaletteManager>();
    auto paletteLoad = _paletteManager->TryLoadPalettes();
    if (!paletteLoad) {
        Log::CoreError(paletteLoad.error().message);
        return;
    }

    _fractalManager = std::make_shared<FractalManager>(_scene.ViewportPane().ViewportSize());

    _camera.ApplyZoom(200.0f);
    _camera.Reset.Subscribe([this]() {
        _camera.ApplyZoom(200.0f);
        return false;
    });
    _camera.Disable();
    _ready = true;
}

void ProjectLayer::OnDetach() {
    BaseLayer::OnDetach();
}

void ProjectLayer::OnUpdate() {
    BaseLayer::OnUpdate();
    if (!_ready) {
        return;
    }

    _paletteManager->OnUpdate();
    _fractalManager->OnUpdate(_scene);
    _fractalManager->OnRender(_scene);
}

void ProjectLayer::OnGuiRender() {
    BaseLayer::OnGuiRender();
    if (!_ready) {
        return;
    }

    if (ImGui::Begin("Project")) {
        _fractalManager->OnGuiRender();
    }
    ImGui::End();
}

void ProjectLayer::OnRenderTargetResize(const sf::Vector2f& newSize) {
    BaseLayer::OnRenderTargetResize(newSize);
    if (!_ready) {
        return;
    }
    _fractalManager->OnViewportResize(newSize);
    _scene.OnRenderTargetResize(newSize);
}
} // namespace fractals
