#include "FractalSet.h"

#include <glad/glad.h>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace Se
{
FractalSet::FractalSet(String name, FractalSetType type, sf::Vector2f renderSize) :
	_name(Move(name)),
	_type(type),
	_computeIterations(64),
	_nWorkerComplete(0),
	_simWidth(renderSize.x),
	_simHeight(renderSize.y),
	_simBox(VecUtils::Null<double>(), VecUtils::Null<double>()),
	_desiredSimulationDimensions(renderSize),
	_vertexArray(sf::PrimitiveType::Points, _simWidth * _simHeight),
	_fractalArray(new int[_simWidth * _simHeight]),
	_painterPS(ShaderStore::Get("painter.frag", sf::Shader::Type::Fragment)),
	_recomputeImage(true),
	_reconstructImage(true),
	_shouldResize(false),
	_desiredPalette(FractalSetPalette::Fiery),
	_colorTransitionTimer(0.0f),
	_colorTransitionDuration(0.7f),
	_palettes(5),
	_axisVA(sf::PrimitiveType::Lines)
{
	_palettes[static_cast<int>(FractalSetPalette::Fiery)] = ImageStore::Get("Pals/fieryRec.png");
	_palettes[static_cast<int>(FractalSetPalette::FieryAlt)] = ImageStore::Get("Pals/fiery.png");
	_palettes[static_cast<int>(FractalSetPalette::UV)] = ImageStore::Get("Pals/uvRec.png");
	_palettes[static_cast<int>(FractalSetPalette::GreyScale)] = ImageStore::Get("Pals/greyscaleRec.png");
	_palettes[static_cast<int>(FractalSetPalette::Rainbow)] = ImageStore::Get("Pals/rainbowRec.png");

	_currentPalette.create(PaletteWidth, 1, _palettes[static_cast<int>(_desiredPalette)]->getPixelsPtr());

	for (int i = 0; i < PaletteWidth; i++)
	{
		const auto pix = _currentPalette.getPixel(i, 0);
		_colorsStart[i] = {
			static_cast<float>(pix.r) / 255.0f, static_cast<float>(pix.g) / 255.0f, static_cast<float>(pix.b) / 255.0f,
			static_cast<float>(pix.a) / 255.0f
		};
	}
	_colorsCurrent = _colorsStart;

	for (size_t i = 0; i < _vertexArray.getVertexCount(); i++)
	{
		_vertexArray[i].position = sf::Vector2f(static_cast<float>(std::floor(i % _simWidth)),
		                                        static_cast<float>(std::floor(i / _simWidth)));
		_vertexArray[i].color = sf::Color{0, 0, 0, 255};
	}

	_outputPS.create(_simWidth, _simHeight);

	_outputCS.create(_simWidth, _simHeight);
	glBindTexture(GL_TEXTURE_2D, _outputCS.getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	_shaderBasedHostTarget.create(_simWidth, _simHeight);
	glBindTexture(GL_TEXTURE_2D, _shaderBasedHostTarget.getTexture().getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	_paletteTexture.create(PaletteWidth, 1);
	glBindTexture(GL_TEXTURE_2D, _paletteTexture.getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, PaletteWidth, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);


	_axisVA.append(sf::Vertex(sf::Vector2f(-3.0f, 0.0f), sf::Color::White));
	_axisVA.append(sf::Vertex(sf::Vector2f(3.0f, 0.0f), sf::Color::White));
	_axisVA.append(sf::Vertex(sf::Vector2f(0.0f, -3.0f), sf::Color::White));
	_axisVA.append(sf::Vertex(sf::Vector2f(0.0f, 3.0f), sf::Color::White));

	constexpr float lineOffset1 = 0.1f;
	constexpr float lineOffset10 = 0.01f;
	constexpr float lineOffset100 = 0.001f;
	constexpr float lineOffset1000 = 0.0001f;

	const int range = 3000;
	for (int i = -range; i <= range; i++)
	{
		float offset = lineOffset1000;
		if (i % 1000 == 0)
		{
			offset = lineOffset1;
		}
		else if (i % 100 == 0)
		{
			offset = lineOffset10;
		}
		else if (i % 10 == 0)
		{
			offset = lineOffset100;
		}

		_axisVA.append(sf::Vertex(sf::Vector2f(static_cast<float>(i) / 1000.0f, -offset), sf::Color::White));
		_axisVA.append(sf::Vertex(sf::Vector2f(static_cast<float>(i) / 1000.0f, offset), sf::Color::White));
		_axisVA.append(sf::Vertex(sf::Vector2f(-offset, static_cast<float>(i) / 1000.0f), sf::Color::White));
		_axisVA.append(sf::Vertex(sf::Vector2f(offset, static_cast<float>(i) / 1000.0f), sf::Color::White));
	}

	FractalSet::OnViewportResize(sf::Vector2f(_simWidth, _simHeight));
}

FractalSet::~FractalSet()
{
	for (auto& worker : _workers)
	{
		worker->Alive = false;
		worker->CvStart.notify_all();
		if (worker->Thread.joinable()) worker->Thread.join();
		worker->FractalArray = nullptr;
		delete worker;
		worker = nullptr;
	}
	delete[] _fractalArray;
	_fractalArray = nullptr;
}

void FractalSet::OnUpdate(Scene& scene)
{
	const auto dt = Global::Clock::FrameTime();
	const auto start = Global::Clock::SinceStart();


	if (_generationType == FractalSetGenerationType::DelayedGeneration)
	{
		if (start - _lastGeneration > sf::seconds(1.0f))
		{
			_lastGeneration = start;
		}
		else
		{
			return;
		}
	}

	UpdatePaletteTexture();

	if (_colorTransitionTimer <= _colorTransitionDuration)
	{
		const float delta = (std::sin((_colorTransitionTimer / _colorTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) /
			2.0f;
		for (int x = 0; x < PaletteWidth; x++)
		{
			const auto pix = _palettes[static_cast<int>(_desiredPalette)]->getPixel(x, 0);
			const TransitionColor goalColor = {
				static_cast<float>(pix.r) / 255.0f, static_cast<float>(pix.g) / 255.0f,
				static_cast<float>(pix.b) / 255.0f, static_cast<float>(pix.a) / 255.0f
			};
			const auto& startColor = _colorsStart[x];
			auto& currentColor = _colorsCurrent[x];
			currentColor.r = startColor.r + delta * (goalColor.r - startColor.r);
			currentColor.g = startColor.g + delta * (goalColor.g - startColor.g);
			currentColor.b = startColor.b + delta * (goalColor.b - startColor.b);
			_currentPalette.setPixel(x, 0, {
				                         static_cast<sf::Uint8>(currentColor.r * 255.0f),
				                         static_cast<sf::Uint8>(currentColor.g * 255.0f),
				                         static_cast<sf::Uint8>(currentColor.b * 255.0f),
				                         static_cast<sf::Uint8>(currentColor.a * 255.0f)
			                         });
		}
		MarkForImageRendering();
		_colorTransitionTimer += dt.asSeconds();
	}
	
	ComputeImage();
	RenderImage();
}

void FractalSet::OnRender(Scene& scene)
{
	scene.ActivateScreenSpaceDrawing();
	switch (_computeHost)
	{
	case FractalSetComputeHost::CPU:
	{
		scene.Submit(_vertexArray);
		break;
	}
	case FractalSetComputeHost::GPUComputeShader:
	case FractalSetComputeHost::GPUPixelShader:
	{
		scene.Submit(sf::Sprite(_shaderBasedHostTarget.getTexture()));
		break;
	}
	}
	scene.DeactivateScreenSpaceDrawing();

	if (_drawAxis)
	{
		scene.Submit(_axisVA);
	}
}

void FractalSet::OnViewportResize(const sf::Vector2f& size)
{
	_blackColorCache.resize(size.x * size.y);
	for (auto& color : _blackColorCache)
	{
		color = sf::Color::Black;
	}
}

void FractalSet::MarkForImageRendering() noexcept
{
	_reconstructImage = true;
}

void FractalSet::MarkForImageComputation() noexcept
{
	_recomputeImage = true;
}

void FractalSet::AddWorker(Worker* worker)
{
	worker->WorkerComplete = &_nWorkerComplete;
	worker->FractalArray = _fractalArray;
	worker->SimWidth = _simWidth;
	worker->Alive = true;
	worker->Thread = Thread(&Worker::Compute, worker);
	_workers.push_back(worker);
}

auto FractalSet::Name() const noexcept -> const String&
{
	return _name;
}

auto FractalSet::Type() const -> FractalSetType
{
	return _type;
}

auto FractalSet::ComputeHost() const -> FractalSetComputeHost
{
	return _computeHost;
}

void FractalSet::SetComputeHost(FractalSetComputeHost computeHost)
{
	_computeHost = computeHost;
	// Force resize as resize is only applied to the current compute host
	_shouldResize = true;
}

void FractalSet::Resize(const sf::Vector2f& size)
{
	_desiredSimulationDimensions = size;
	_shouldResize = true;
}

auto FractalSet::PaletteTexture() const -> const sf::Texture&
{
	return _paletteTexture;
}

void FractalSet::SetSimBox(const SimBox& box)
{
	_simBox = box;
}

void FractalSet::SetComputeIterationCount(size_t iterations) noexcept
{
	_computeIterations = iterations;
}

void FractalSet::SetPalette(FractalSetPalette palette) noexcept
{
	_desiredPalette = palette;
	_colorTransitionTimer = 0.0f;
	_colorsStart = _colorsCurrent;
}

auto FractalSet::GenerationType() const -> FractalSetGenerationType
{
	return _generationType;
}

void FractalSet::SetGenerationType(FractalSetGenerationType type)
{
	_lastGeneration = Global::Clock::SinceStart();
	_generationType = type;
	MarkForImageComputation();
	MarkForImageRendering();
}

void FractalSet::ActivateAxis()
{
	_drawAxis = true;
}

void FractalSet::DeactivateAxis()
{
	_drawAxis = false;
}

void FractalSet::SetUniform(uint id, const String& name, const sf::Vector2<double>& value)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	Debug::Assert(loc != -1);
	glUniform2d(loc, value.x, value.y);

	glUseProgram(0);
}

void FractalSet::SetUniform(uint id, const String& name, float value)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	Debug::Assert(loc != -1);
	glUniform1f(loc, value);

	glUseProgram(0);
}

void FractalSet::SetUniform(uint id, const String& name, double value)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	Debug::Assert(loc != -1);
	glUniform1d(loc, value);

	glUseProgram(0);
}

void FractalSet::SetUniform(uint id, const String& name, int value)
{
	glUseProgram(id);

	const auto loc = glGetUniformLocation(id, name.c_str());
	Debug::Assert(loc != -1);
	glUniform1i(loc, value);

	glUseProgram(0);
}

void FractalSet::UpdatePaletteTexture()
{
	glBindTexture(GL_TEXTURE_2D, _paletteTexture.getNativeHandle());
	const auto size = _paletteTexture.getSize();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
	             _currentPalette.getPixelsPtr());
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FractalSet::ComputeImage()
{
	if (_recomputeImage)
	{
		if (_shouldResize)
		{
			_simWidth = _desiredSimulationDimensions.x;
			_simHeight = _desiredSimulationDimensions.y;

			switch (_computeHost)
			{
			case FractalSetComputeHost::CPU:
			{
				_vertexArray.resize(_simWidth * _simHeight);
				for (size_t i = 0; i < _vertexArray.getVertexCount(); i++)
				{
					_vertexArray[i].position = sf::Vector2f(static_cast<float>(std::floor(i % _simWidth)),
					                                        static_cast<float>(std::floor(i / _simWidth)));
					_vertexArray[i].color = sf::Color{0, 0, 0, 255};
				}

				//delete[] _fractalArray;
				_fractalArray = new int[_simWidth * _simHeight];
				for (auto* worker : _workers)
				{
					worker->FractalArray = _fractalArray;
					worker->SimWidth = _simWidth;
				}
				break;
			}

			case FractalSetComputeHost::GPUComputeShader:
			{
				if (_simWidth != _outputCS.getSize().x || _simHeight != _outputCS.getSize().y)
				{
					_outputCS.create(_simWidth, _simHeight);
					_shaderBasedHostTarget.create(_simWidth, _simHeight);

					glBindTexture(GL_TEXTURE_2D, _outputCS.getNativeHandle());
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					             nullptr);

					glBindTexture(GL_TEXTURE_2D, _shaderBasedHostTarget.getTexture().getNativeHandle());
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					             nullptr);

					glBindTexture(GL_TEXTURE_2D, 0);
				}
				break;
			}
			case FractalSetComputeHost::GPUPixelShader:
			{
				if (_simWidth != _outputPS.getSize().x || _simHeight != _outputPS.getSize().y)
				{
					_outputPS.create(_simWidth, _simHeight);
					_shaderBasedHostTarget.create(_simWidth, _simHeight);

					glBindTexture(GL_TEXTURE_2D, _outputPS.getTexture().getNativeHandle());
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					             nullptr);

					glBindTexture(GL_TEXTURE_2D, 0);
				}
				break;
			}
			}

			_shouldResize = false;
		}

		switch (_computeHost)
		{
		case FractalSetComputeHost::CPU:
		{
			const double imageSectionWidth = static_cast<double>(_simWidth) / static_cast<double>(_workers.size());
			const double fractalSectionWidth = (_simBox.BottomRight.x - _simBox.TopLeft.x) / static_cast<double>(
				_workers.size());

			_nWorkerComplete = 0;

			for (size_t i = 0; i < _workers.size(); i++)
			{
				_workers[i]->ImageTL = sf::Vector2(imageSectionWidth * i, 0.0);
				_workers[i]->ImageBR = sf::Vector2<double>(imageSectionWidth * static_cast<double>(i + 1), _simHeight);
				_workers[i]->FractalTL = sf::Vector2(_simBox.TopLeft.x + i * fractalSectionWidth, _simBox.TopLeft.y);
				_workers[i]->FractalBR = sf::Vector2(
					_simBox.TopLeft.x + fractalSectionWidth * static_cast<double>(i + 1), _simBox.BottomRight.y);
				_workers[i]->Iterations = _computeIterations;

				std::unique_lock lm(_workers[i]->Mutex);
				_workers[i]->CvStart.notify_one();
			}

			while (_nWorkerComplete < _workers.size()) // Wait for all workers to complete
			{
			}
			break;
		}
		case FractalSetComputeHost::GPUComputeShader:
		{
			// Clears texture
			glBindTexture(GL_TEXTURE_2D, _outputCS.getNativeHandle());
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _simWidth, _simHeight, GL_RGBA, GL_UNSIGNED_BYTE,
			                _blackColorCache.data());
			glBindTexture(GL_TEXTURE_2D, 0);

			glBindImageTexture(0, _outputCS.getNativeHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
			UpdateComputeShaderUniforms();
			auto fractalSetCS = ComputeShader();

			const auto workerDim = ComputeShaderWorkerDim();
			fractalSetCS->Dispatch(workerDim.x, workerDim.y, 1);
			ComputeShader::AwaitFinish();

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


			break;
		}
		case FractalSetComputeHost::GPUPixelShader:
		{
			UpdatePixelShaderUniforms();
			const auto pixelShader = PixelShader();

			sf::RectangleShape simRectShape(sf::Vector2f(_simWidth, _simHeight));
			simRectShape.setTexture(&_paletteTexture);
			_outputPS.draw(simRectShape, {pixelShader.get()});
			break;
		}
		}
		_recomputeImage = false;
	}
}

void FractalSet::RenderImage()
{
	if (_reconstructImage)
	{
		switch (_computeHost)
		{
		case FractalSetComputeHost::CPU:
		{
			const auto* const colorPal = _currentPalette.getPixelsPtr();
			for (int y = 0; y < _simHeight; y++)
			{
				for (int x = 0; x < _simWidth; x++)
				{
					const int i = _fractalArray[y * _simWidth + x];
					const float offset = static_cast<float>(i) / static_cast<float>(_computeIterations) * static_cast<
						float>(PaletteWidth - 1);


					memcpy(&_vertexArray[y * _simWidth + x].color, &colorPal[static_cast<int>(offset) * 4],
					       sizeof(sf::Uint8) * 3);
				}
			}
			break;
		}
		case FractalSetComputeHost::GPUComputeShader:
		case FractalSetComputeHost::GPUPixelShader:
		{
			const auto outputHandle = _computeHost == FractalSetComputeHost::GPUComputeShader
				                          ? _outputCS.getNativeHandle()
				                          : _outputPS.getTexture().getNativeHandle();

			glBindImageTexture(0, outputHandle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glBindImageTexture(1, _paletteTexture.getNativeHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			SetUniform(_painterPS->getNativeHandle(), "maxPixelValue", static_cast<float>(_computeIterations));
			SetUniform(_painterPS->getNativeHandle(), "paletteWidth", PaletteWidth);


			// Tmp
			sf::RectangleShape black(sf::Vector2f(_simWidth, _simHeight));
			black.setFillColor(sf::Color::Black);
			_shaderBasedHostTarget.draw(black);

			sf::RectangleShape simRectShape(sf::Vector2f(_simWidth, _simHeight));
			simRectShape.setTexture(&_paletteTexture);
			_shaderBasedHostTarget.draw(simRectShape, {_painterPS.get()});
			break;
		}
		}
		_reconstructImage = false;
	}
}
}
