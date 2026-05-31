#include "palette_manager.h"

#include "glad/glad.h"

namespace fractals {
using namespace saffron;
PaletteManager::PaletteManager() : Singleton(this), _desired(PaletteType::Fiery) {
    // Create and initial upload
    if (!_texture.create(PaletteWidth, 1)) {
        Log::CoreError("Failed to create palette texture {0}x1", PaletteWidth);
    }
}

auto PaletteManager::TryLoadPalettes() -> Status {
    const std::array<std::pair<PaletteType, std::filesystem::path>, 5> paletteFiles = {{
        {PaletteType::Fiery, "pals/fieryRec.png"},
        {PaletteType::FieryAlt, "pals/fiery.png"},
        {PaletteType::UV, "pals/uvRec.png"},
        {PaletteType::GreyScale, "pals/greyscaleRec.png"},
        {PaletteType::Rainbow, "pals/rainbowRec.png"},
    }};

    for (const auto& [type, path] : paletteFiles) {
        auto image = ImageStore::TryGet(path, false);
        if (!image) {
            return std::unexpected(image.error());
        }
        _palettes.emplace(type, *image);
    }

    for (const auto& image : _palettes | std::views::values) {
        if (image == nullptr || image->getSize().x < PaletteWidth || image->getSize().y < 1) {
            return std::unexpected(Error{ErrorCode::Resource, "Palette image is missing or too small"});
        }
    }

    _currentPalette.create(PaletteWidth, 1, _palettes.at(_desired)->getPixelsPtr());

    for (int i = 0; i < PaletteWidth; i++) {
        const auto pix = _currentPalette.getPixel(i, 0);
        _colorsStart[i] = {static_cast<float>(pix.r) / 255.0f, static_cast<float>(pix.g) / 255.0f,
                           static_cast<float>(pix.b) / 255.0f, static_cast<float>(pix.a) / 255.0f};
    }
    _colorsCurrent = _colorsStart;
    _ready = true;
    return {};
}

auto PaletteManager::Ready() const -> bool {
    return _ready;
}

void PaletteManager::OnUpdate() {
    if (!_ready) {
        return;
    }

    if (_wantTextureUpload) {
        glBindTexture(GL_TEXTURE_2D, _texture.getNativeHandle());
        const auto size = _texture.getSize();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     _currentPalette.getPixelsPtr());
        glBindTexture(GL_TEXTURE_2D, 0);
        _wantTextureUpload = false;
    }

    if (_colorTransitionTimer <= _colorTransitionDuration) {
        const float delta =
            (std::sin((_colorTransitionTimer / _colorTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) / 2.0f;
        for (int x = 0; x < PaletteWidth; x++) {
            const auto pix = _palettes.at(_desired)->getPixel(x, 0);
            const TransitionColor goalColor = {static_cast<float>(pix.r) / 255.0f, static_cast<float>(pix.g) / 255.0f,
                                               static_cast<float>(pix.b) / 255.0f, static_cast<float>(pix.a) / 255.0f};
            const auto& startColor = _colorsStart[x];
            auto& currentColor = _colorsCurrent[x];
            currentColor.r = startColor.r + delta * (goalColor.r - startColor.r);
            currentColor.g = startColor.g + delta * (goalColor.g - startColor.g);
            currentColor.b = startColor.b + delta * (goalColor.b - startColor.b);
            _currentPalette.setPixel(
                x, 0,
                {static_cast<sf::Uint8>(currentColor.r * 255.0f), static_cast<sf::Uint8>(currentColor.g * 255.0f),
                 static_cast<sf::Uint8>(currentColor.b * 255.0f), static_cast<sf::Uint8>(currentColor.a * 255.0f)});
        }

        PaletteUpdated.Invoke();
        _colorTransitionTimer += Global::Clock::FrameTime().asSeconds();
    }
}

void PaletteManager::UploadTexture() {
    _wantTextureUpload = true;
}

auto PaletteManager::Texture() const -> const sf::Texture& {
    return _texture;
}

auto PaletteManager::Desired() const -> PaletteType {
    return _desired;
}

auto PaletteManager::DesiredPixelPtr() const -> const sf::Uint8* {
    if (!_ready) {
        return nullptr;
    }
    return _currentPalette.getPixelsPtr();
}

auto PaletteManager::DesiredImage() const -> const sf::Image& {
    return *_palettes.at(_desired);
}

void PaletteManager::SetActive(PaletteType type) {
    if (!_ready) {
        return;
    }
    _desired = type;
    _colorTransitionTimer = 0.0f;
    _colorsStart = _colorsCurrent;
}
} // namespace fractals
