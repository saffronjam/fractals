#pragma once

#include "glad/glad.h"

#include <Saffron.h>

namespace Se
{
using RealType = double;
using Position = sf::Vector2<double>;

inline void ExecuteIfFound(uint id, const std::string& name, const std::function<void(int)>& shaderOp)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	if (loc == -1)
	{
		Log::Warn("Could not found shader uniform \"" + name + "\". Check spelling or if it was optimized away");
	}
	else
	{
		shaderOp(loc);
	}

	glUseProgram(0);
}

static void SetUniform(uint id, const std::string& name, const std::array<double, 4>& value)
{
	ExecuteIfFound(id, name, [&](int loc)
	{
		glUniform4d(loc, value[0], value[1], value[2], value[3]);
	});
}

static void SetUniform(uint id, const std::string& name, const sf::Vector4<double>& value)
{
	ExecuteIfFound(id, name, [&](int loc)
	{
		glUniform4d(loc, value.x, value.y, value.z, value.w);
	});
}

static void SetUniform(uint id, const std::string& name, const sf::Vector2<double>& value)
{
	ExecuteIfFound(id, name, [&](int loc)
	{
		glUniform2d(loc, value.x, value.y);
	});
}

static void SetUniform(uint id, const std::string& name, float value)
{
	ExecuteIfFound(id, name, [&](int loc)
	{
		glUniform1f(loc, value);
	});
}

static void SetUniform(uint id, const std::string& name, double value)
{
	ExecuteIfFound(id, name, [&](int loc)
	{
		glUniform1d(loc, value);
	});
}

static void SetUniform(uint id, const std::string& name, int value)
{
	ExecuteIfFound(id, name, [&](int loc)
	{
		glUniform1i(loc, value);
	});
}
}
