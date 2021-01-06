#pragma once

#include <random>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

namespace Se
{

template<typename T>
using UniformRealDistribution = std::uniform_real_distribution<T>;
template<typename T>
using UniformIntDistribution = std::uniform_int_distribution<T>;


class Random
{
public:
    using Device = std::random_device;
    using Engine = std::mt19937;

public:
    template<typename IntegralType>
    static int
    Integer(IntegralType lower = static_cast<IntegralType>(0), IntegralType upper = static_cast<IntegralType>(100))
    {
        static_assert(std::is_integral<IntegralType>::value, "IntegralType must be integral type");
        static Device s_Device;
        static Engine s_Engine(s_Device());
        UniformIntDistribution<IntegralType> distribution(lower, upper);
        return distribution(s_Engine);
    }

    template<typename RealType = float>
    static RealType Real(RealType lower = static_cast<RealType>(0), RealType upper = static_cast<RealType>(1))
    {
        static_assert(std::is_floating_point<RealType>::value, "RealType must be floating point type");
        static Device s_Device;
        static Engine s_Engine(s_Device());
        UniformRealDistribution<RealType> distribution(lower, upper);
        return distribution(s_Engine);
    }

    template<typename NumberType>
    static sf::Vector3<NumberType> Vec3(const sf::Vector3<NumberType> &low, const sf::Vector3<NumberType> &high)
    {
        return Vec3(low.x, low.y, low.z, high.x, high.y, high.z);
    }

    template<typename NumberType>
    static sf::Vector3<NumberType>
    Vec3(NumberType lowX, NumberType lowY, NumberType lowZ, NumberType highX, NumberType highY, NumberType highZ)
    {
        static_assert(std::is_arithmetic<NumberType>::value, "NumberType must be arithmetic type");
        if constexpr (std::is_integral<NumberType>::value)
        {
            float x = Integer<NumberType>(lowX, highX);
            float y = Integer<NumberType>(lowY, highY);
            float z = Integer<NumberType>(lowZ, highZ);
            return {x, y, z};
        }
        else
        {
            float x = Real<NumberType>(lowX, highX);
            float y = Real<NumberType>(lowY, highY);
            float z = Real<NumberType>(lowZ, highZ);
            return {x, y, z};
        }
    }
};

template<typename T>
class RandomGenerator : Random
{
public:
    explicit RandomGenerator(T lower = static_cast<T>(0), T upper = static_cast<T>(100)) :
            m_Lower(lower),
            m_Upper(upper) {};

    T Generate()
    {
        static Device s_RandomDevice;
        static Engine s_Engine(s_RandomDevice());

        return static_cast<T>(GetUniformDistribution()(s_Engine));
    }

    void SetLower(T lower) { m_Lower = lower; };

    void SetUpper(T upper) { m_Upper = upper; };

private:
    auto &GetUniformDistribution()
    {
        static_assert(std::is_arithmetic<T>::value);
        if constexpr (std::is_integral<T>::value)
        {
            static UniformIntDistribution<T> s_Distribution;
            return s_Distribution;
        }
        else
        {
            static UniformRealDistribution<T> s_Distribution;
            return s_Distribution;
        }
    }

    T m_Lower, m_Upper;
};

}