#pragma once

#include <SFML/System/Vector2.hpp>

#include "Saffron/Base.h"
#include "Saffron/Math/SaffronMath.h"

namespace Se
{
class ComputeShader
{
public:
	void Bind();
	void Unbind();

	void Dispatch(Uint32 xGroup, Uint32 yGroup, Uint32 zGroup);

	bool LoadFromFile(const Filepath& filepath);

	void SetFloat(const String& name, float value);
	void SetDouble(const String& name, double value);
	void SetInt(const String& name, int value);;

	void SetVector2d(const String& name, const sf::Vector2<double>& value);
	void SetVector2d(const String& name, const Complex<double>& value);
	void SetVector4f(const String& name, const sf::Vector4f& value);

	static void AwaitFinish();

private:
	void BindThenUnbind(const Function<void()>& fn);

private:
	Uint32 _id{};
};
}
