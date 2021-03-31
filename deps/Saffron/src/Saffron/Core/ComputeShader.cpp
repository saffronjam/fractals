#include "SaffronPCH.h"

#include <Glad/glad.h>

#include "Saffron/Core/ComputeShader.h"
#include "Saffron/Core/FileIOManager.h"

namespace Se
{
void ComputeShader::Bind()
{
	glUseProgram(_id);
}

void ComputeShader::Unbind()
{
	glUseProgram(0);
}

void ComputeShader::Dispatch(Uint32 xGroup, Uint32 yGroup, Uint32 zGroup)
{
	BindThenUnbind([&]
	{
		glDispatchCompute(xGroup, yGroup, zGroup);
	});
}

bool ComputeShader::LoadFromFile(const Filepath& filepath)
{
	SE_CORE_ASSERT(FileIOManager::FileExists(filepath));

	String content;
	InputStream fileStream(filepath, std::ios::in);

	if (!fileStream.is_open())
	{
		std::cerr << "Could not read file " << filepath << ". File does not exist." << std::endl;
	}

	String line;
	while (!fileStream.eof())
	{
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();

	const auto* contentCStr = content.c_str();
	const auto shader = glCreateShader(GL_COMPUTE_SHADER);
	if (shader < 1)
	{
		return false;
	}

	glShaderSource(shader, 1, &contentCStr, nullptr);
	// Try compile
	{
		glCompileShader(shader);
		int status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			return false;
		}
	}

	_id = glCreateProgram();
	if (_id < 1)
	{
		return false;
	}
	glAttachShader(_id, shader);
	// Try link
	{
		glLinkProgram(_id);
		int status;
		glGetShaderiv(shader, GL_LINK_STATUS, &status);
		if (status == 0)
		{
			_id = 0;
			return false;
		}
	}

	return true;
}

void ComputeShader::SetFloat(const String& name, float value)
{
	BindThenUnbind([&]
	{
		const auto loc = glGetUniformLocation(_id, name.c_str());
		SE_CORE_ASSERT(loc != -1);
		glUniform1f(loc, value);
	});
}

void ComputeShader::SetDouble(const String& name, double value)
{
	BindThenUnbind([&]
	{
		const auto loc = glGetUniformLocation(_id, name.c_str());
		SE_CORE_ASSERT(loc != -1);
		glUniform1d(loc, value);
	});
}

void ComputeShader::SetInt(const String& name, int value)
{
	BindThenUnbind([&]
	{
		const auto loc = glGetUniformLocation(_id, name.c_str());
		SE_CORE_ASSERT(loc != -1);
		glUniform1i(loc, value);
	});
}

void ComputeShader::SetVector2d(const String& name, const sf::Vector2<double>& value)
{
	BindThenUnbind([&]
	{
		const auto loc = glGetUniformLocation(_id, name.c_str());
		SE_CORE_ASSERT(loc != -1);
		glUniform2d(loc, value.x, value.y);
	});
}

void ComputeShader::SetVector2d(const String& name, const Complex<double>& value)
{
	SetVector2d(name, sf::Vector2<double>(value.real(), value.imag()));
}

void ComputeShader::SetVector4f(const String& name, const sf::Vector4f& value)
{
	BindThenUnbind([&]
	{
		const auto loc = glGetUniformLocation(_id, name.c_str());
		SE_CORE_ASSERT(loc != -1);
		glUniform4f(loc, value.x, value.y, value.z, value.w);
	});
}

void ComputeShader::AwaitFinish()
{
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeShader::BindThenUnbind(const Function<void()>& fn)
{
	Bind();
	fn();
	Unbind();
}
}
