#pragma once

#include <SFML/Graphics/Color.hpp>

#include "Saffron/Base.h"

namespace Se
{
class FadePane : public Signaller
{
public:
    struct Signals
    {
        static SignalAggregate<void> OnFinish;
    };

    using FadeFn = Function<Uint8(sf::Time, sf::Time)>;

    enum class Type
    {
        Out = 0,
        In = 1
    };

public:
    FadePane(Type type,
             sf::Time duration,
             sf::Time delay = sf::Time::Zero,
             bool startOnCreation = false,
             sf::Color color = sf::Color::Black);
    FadePane(Type type,
             sf::Time duration,
             FadeFn alphaFunction,
             sf::Time delay = sf::Time::Zero,
             bool startOnCreation = false,
             sf::Color color = sf::Color::Black);

    void OnUpdate();
    void OnGuiRender();

    void Start();
    bool IsActive() const { return _wantFade; }

private:
    Uint8 DefaultAlphaFunction(sf::Time timer, sf::Time duration);

private:
    Type _type;

    FadeFn _alphaFunction;

    bool _wantFade = false;
    sf::Color _fadeColor;

    sf::Time _timer = sf::Time::Zero;
    const sf::Time _duration;

    sf::Time _delayTimer = sf::Time::Zero;
    const sf::Time _delay;
};
}
