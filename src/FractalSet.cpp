#include "FractalSet.h"

#include <glad/glad.h>

namespace Se
{
FractalSet::FractalSet(String name, Type type, sf::Vector2f renderSize) :
	_name(Move(name)),
	_type(type),
	_computeIterations(64),
	_nWorkerComplete(0),
	_simWidth(renderSize.x),
	_simHeight(renderSize.y),
	_simBox(VecUtils::Null<double>(), VecUtils::Null<double>()),
	_painterCS(ComputeShaderStore::Get("painter.comp")),
	_vertexArray(sf::PrimitiveType::Points, _simWidth * _simHeight),
	_fractalArray(new int[_simWidth * _simHeight]),
	_desiredSimulationDimensions(renderSize),
	_recomputeImage(true),
	_reconstructImage(true),
	_shouldResize(false),
	_desiredPalette(Fiery),
	_colorTransitionTimer(0.0f),
	_colorTransitionDuration(0.7f),
	_palettes(4)
{
	_palettes[Fiery] = ImageStore::Get("Pals/fiery.png");
	_palettes[UV] = ImageStore::Get("Pals/uv.png");
	_palettes[GreyScale] = ImageStore::Get("Pals/greyscale.png");
	_palettes[Rainbow] = ImageStore::Get("Pals/rainbow.png");

	_currentPalette.create(PaletteWidth, 1, _palettes[_desiredPalette]->getPixelsPtr());

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

	_output.create(_simWidth, _simHeight);
	_data.create(_simWidth, _simHeight);
	_paletteTexture.create(PaletteWidth, 1);

	glBindTexture(GL_TEXTURE_2D, _output.getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, _data.getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, _paletteTexture.getNativeHandle());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, PaletteWidth, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

FractalSet::~FractalSet()
{
	for (auto& worker : _workers)
	{
		worker->alive = false;
		worker->cvStart.notify_all();
		if (worker->thread.joinable()) worker->thread.join();
		worker->fractalArray = nullptr;
		delete worker;
		worker = nullptr;
	}
	delete[] _fractalArray;
	_fractalArray = nullptr;
}

void FractalSet::OnUpdate(Scene& scene)
{
	UpdatePaletteTexture();

	glBindImageTexture(0, _output.getNativeHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(1, _data.getNativeHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
	glBindImageTexture(2, _paletteTexture.getNativeHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);


	if (_colorTransitionTimer <= _colorTransitionDuration)
	{
		const float delta = (std::sin((_colorTransitionTimer / _colorTransitionDuration) * PI<> - PI<> / 2.0f) + 1.0f) /
			2.0f;
		for (int x = 0; x < PaletteWidth; x++)
		{
			const auto pix = _palettes[_desiredPalette]->getPixel(x, 0);
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
		_colorTransitionTimer += Global::Clock::GetFrameTime().asSeconds();
	}
	ComputeImage();
	RenderImage();
}

void FractalSet::OnRender(Scene& scene)
{
	if (_computeHost == ComputeHost::CPU)
	{
		scene.Submit(_vertexArray);
	}
	else if (_computeHost == ComputeHost::GPU)
	{
		scene.Submit(sf::Sprite(_output));
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
	worker->nWorkerComplete = &_nWorkerComplete;
	worker->fractalArray = _fractalArray;
	worker->simWidth = _simWidth;
	worker->alive = true;
	worker->thread = Thread(&Worker::Compute, worker);
	_workers.push_back(worker);
}

const String& FractalSet::GetName() const noexcept
{
	return _name;
}

FractalSet::Type FractalSet::GetType() const
{
	return _type;
}

FractalSet::ComputeHost FractalSet::GetComputeHost() const
{
	return _computeHost;
}

void FractalSet::SetComputeHost(ComputeHost computeHost)
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

void FractalSet::SetSimBox(const SimBox& box)
{
	_simBox = box;
}

void FractalSet::SetComputeIterationCount(size_t iterations) noexcept
{
	_computeIterations = iterations;
}

void FractalSet::SetPalette(Palette palette) noexcept
{
	_desiredPalette = palette;
	_colorTransitionTimer = 0.0f;
	_colorsStart = _colorsCurrent;
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

			if (_computeHost == ComputeHost::CPU)
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
					worker->fractalArray = _fractalArray;
					worker->simWidth = _simWidth;
				}
			}
			else if (_computeHost == ComputeHost::GPU)
			{
				if (_simWidth != _output.getSize().x || _simHeight != _output.getSize().y)
				{
					_output.create(_simWidth, _simHeight);
					_data.create(_simWidth, _simHeight);

					glBindTexture(GL_TEXTURE_2D, _output.getNativeHandle());
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					             nullptr);
					glBindTexture(GL_TEXTURE_2D, _data.getNativeHandle());
					glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _simWidth, _simHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
					             nullptr);
				}
			}
			_shouldResize = false;
		}

		if (_computeHost == ComputeHost::CPU)
		{
			const double imageSectionWidth = static_cast<double>(_simWidth) / static_cast<double>(_workers.size());
			const double fractalSectionWidth = static_cast<double>(_simBox.BottomRight.x - _simBox.TopLeft.x) /
				static_cast<double>(_workers.size());

			_nWorkerComplete = 0;

			for (size_t i = 0; i < _workers.size(); i++)
			{
				_workers[i]->imageTL = sf::Vector2<double>(imageSectionWidth * i, 0.0f);
				_workers[i]->imageBR = sf::Vector2<double>(imageSectionWidth * static_cast<double>(i + 1), _simHeight);
				_workers[i]->fractalTL = sf::Vector2<double>(
					_simBox.TopLeft.x + static_cast<double>(fractalSectionWidth * i), _simBox.TopLeft.y);
				_workers[i]->fractalBR = sf::Vector2<double>(
					_simBox.TopLeft.x + fractalSectionWidth * static_cast<double>(i + 1), _simBox.BottomRight.y);
				_workers[i]->iterations = _computeIterations;

				std::unique_lock<Mutex> lm(_workers[i]->mutex);
				_workers[i]->cvStart.notify_one();
			}

			while (_nWorkerComplete < _workers.size()) // Wait for all workers to complete
			{
			}
		}
		else if (_computeHost == ComputeHost::GPU)
		{
			UpdateComputeShaderUniforms();
			auto fractalSetCS = GetComputeShader();
			fractalSetCS->Dispatch(_simWidth, _simHeight, 1);

			ComputeShader::AwaitFinish();
		}
		_recomputeImage = false;
	}
}

void FractalSet::RenderImage()
{
	if (_reconstructImage)
	{
		if (_computeHost == ComputeHost::CPU)
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
		}
		else if (_computeHost == ComputeHost::GPU)
		{
			_painterCS->SetFloat("maxPixelValue", _computeIterations);
			_painterCS->SetInt("paletteWidth", PaletteWidth);
			_painterCS->Dispatch(_simWidth, _simHeight, 1);

			ComputeShader::AwaitFinish();
		}
		_reconstructImage = false;
	}
}
}
