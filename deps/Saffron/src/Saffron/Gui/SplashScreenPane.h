#pragma once

#include <SFML/Graphics/Texture.hpp>

#include "Saffron/Base.h"
#include "Saffron/Core/BatchLoader.h"
#include "Saffron/Gui/FadePane.h"

namespace Se
{
class SplashScreenPane
{
public:
    explicit SplashScreenPane(const std::shared_ptr<BatchLoader> &batchLoader);

    void OnUpdate();
    void OnGuiRender();

    const std::shared_ptr<BatchLoader> &GetBatchLoader() const { return _batchLoader; }

    void Show();
    void Hide();
    bool IsIdle() const;
    bool IsFinished() const { return _finished; }
    bool IsHidden() const { return _hidden; }

private:
    String _title = "Saffron Engine 2D";

    std::shared_ptr<BatchLoader> _batchLoader;
    sf::Texture _texture;
    bool _hidden = false;
    bool _finished = false;
    String _finalizingStatus;

    sf::Time _progressTimer = sf::Time::Zero;
    float _progressView = 0.0f;
    float _progressViewFinished = 0.0f;

    sf::Time _holdTimer = sf::Time::Zero;
    const sf::Time _holdTimerFinished = sf::seconds(0.8f);

    FadePane _fadeIn;
    FadePane _fadeOut;
};
}

