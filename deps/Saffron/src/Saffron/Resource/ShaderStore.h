#pragma once

#include <SFML/Graphics/Shader.hpp>

#include "Saffron/Resource/ResourceStore.h"

namespace Se
{
class ShaderStore : public ResourceStore<sf::Shader>
{
public:
	struct ShaderStoreEntry
	{
	};

public:
	static Shared<sf::Shader> Get(const Filepath& vertexShader, const Filepath& pixelShader)
	{
		const Filepath concat = vertexShader.string() + ConcatToken + pixelShader.string();
		return Instance()._Get(concat, false);
	}

	static Shared<sf::Shader> Get(const Filepath& filepath, sf::Shader::Type type)
	{
		switch (type)
		{
		case sf::Shader::Type::Vertex:
		{
			const Filepath concat = filepath.string() + VertexToken;
			return Instance()._Get(concat, false);
		}
		case sf::Shader::Type::Fragment:
		{
			const Filepath concat = filepath.string() + PixelToken;
			return Instance()._Get(concat, false);
		}
		default:
		{
			SE_CORE_FALSE_ASSERT("Shader type not supported in store");
			return nullptr;
		}
		}
	}


private:
	Filepath GetLocation() override
	{
		return "res/Shaders/";
	}

	static ShaderStore& Instance()
	{
		static ShaderStore resourceStore;
		return resourceStore;
	}

private:
	Shared<sf::Shader> Load(Filepath filepath) override
	{
		const auto fpString = filepath.string();

		const auto concatTokenIndex = fpString.find(ConcatToken);
		auto resource = CreateShared<sf::Shader>();
		bool result = false;


		// If vertex shader + pixel shader
		if (concatTokenIndex == String::npos)
		{
			const auto typeToken = fpString.substr(fpString.length() - TypeTokenLength);
			const auto formattedFilepath = fpString.substr(0, fpString.length() - TypeTokenLength);
			if (typeToken == VertexToken)
			{
				result = resource->loadFromFile(formattedFilepath, sf::Shader::Type::Vertex);
			}
			else if (typeToken == PixelToken)
			{
				result = resource->loadFromFile(formattedFilepath, sf::Shader::Type::Fragment);
			}
		}
		else
		{
			const auto pixelShaderFilepath = fpString.substr(0, concatTokenIndex);
			const auto vertexShaderFilepath = fpString.substr(concatTokenIndex + ConcatToken.length());
			result = resource->loadFromFile(pixelShaderFilepath, vertexShaderFilepath);
		}

		SE_CORE_ASSERT(result, "Failed to load ComputeShader");
		return resource;
	}

	inline static const String ConcatToken = "%%";
	inline static const String VertexToken = "%V%";
	inline static const String PixelToken = "%P%";
	static constexpr size_t TypeTokenLength = 3;
};
}
