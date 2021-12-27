#pragma once

#include "glad/glad.h"

#include <Saffron.h>

namespace Se
{
using RealType = double;
using Position = sf::Vector2<double>;

static void SetUniform(uint id, const std::string& name, const sf::Vector2<double>& value)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	Debug::Assert(loc != -1);
	glUniform2d(loc, value.x, value.y);

	glUseProgram(0);
}

static void SetUniform(uint id, const std::string& name, float value)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	Debug::Assert(loc != -1);
	glUniform1f(loc, value);

	glUseProgram(0);
}

static void SetUniform(uint id, const std::string& name, double value)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	Debug::Assert(loc != -1);
	glUniform1d(loc, value);

	glUseProgram(0);
}

static void SetUniform(uint id, const std::string& name, int value)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	Debug::Assert(loc != -1);
	glUniform1i(loc, value);

	glUseProgram(0);
}
}
