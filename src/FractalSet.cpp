#include "FractalSet.h"

namespace Se
{
FractalSet::FractalSet(String name, Type type, sf::Vector2f renderSize) :
	_name(std::move(name)),
	_type(type),
	_computeIterations(64),
	_nWorkerComplete(0),
	_simWidth(renderSize.x),
	_simHeight(renderSize.y),
	_simBox(vl::Null<>(), vl::Null<>()),
	_vertexArray(sf::PrimitiveType::Points, _simWidth *_simHeight),
	_fractalArray(new int[_simWidth * _simHeight]),
	_vertexArrayDimensions(renderSize),
	_recomputeImage(true),
	_reconstructImage(true),
	_shouldResize(false),
	_desiredPalette(Fiery),
	_colorTransitionTimer(0.0f),
	_colorTransitionDuration(0.7f),
	_palettes(4)
{
	_palettes[Fiery] = ImageStore::GetCopy("res/Pals/fiery.png");
	_palettes[UV] = ImageStore::GetCopy("res/Pals/uv.png");
	_palettes[GreyScale] = ImageStore::GetCopy("res/Pals/greyscale.png");
	_palettes[Rainbow] = ImageStore::GetCopy("res/Pals/rainbow.png");

	_currentPalette.create(PaletteWidth, 1, _palettes[_desiredPalette].getPixelsPtr());

	for ( int i = 0; i < PaletteWidth; i++ )
	{
		const auto pix = _currentPalette.getPixel(i, 0);
		_colorsStart[i] = { static_cast<float>(pix.r) / 255.0f, static_cast<float>(pix.g) / 255.0f, static_cast<float>(pix.b) / 255.0f, static_cast<float>(pix.a) / 255.0f };
	}
	_colorsCurrent = _colorsStart;

	for ( size_t i = 0; i < _vertexArray.getVertexCount(); i++ )
	{
		_vertexArray[i].position = sf::Vector2f(static_cast<float>(std::floor(i % _simWidth)), static_cast<float>(std::floor(i / _simWidth)));
		_vertexArray[i].color = sf::Color{ 0, 0, 0, 255 };
	}
}

FractalSet::~FractalSet()
{
	for ( auto &worker : _workers )
	{
		worker->alive = false;
		worker->cvStart.notify_all();
		if ( worker->thread.joinable() )
			worker->thread.join();
		worker->fractalArray = nullptr;
		delete worker;
		worker = nullptr;
	}
	delete[] _fractalArray;
	_fractalArray = nullptr;
}

void FractalSet::OnUpdate(Scene &scene)
{
	if ( _colorTransitionTimer <= _colorTransitionDuration )
	{
		const float delta = (std::sin((_colorTransitionTimer / _colorTransitionDuration) * PI<> -PI<> / 2.0f) + 1.0f) / 2.0f;
		for ( int x = 0; x < PaletteWidth; x++ )
		{
			const auto pix = _palettes[_desiredPalette].getPixel(x, 0);
			const TransitionColor goalColor = { static_cast<float>(pix.r) / 255.0f, static_cast<float>(pix.g) / 255.0f, static_cast<float>(pix.b) / 255.0f, static_cast<float>(pix.a) / 255.0f };
			const auto &startColor = _colorsStart[x];
			auto &currentColor = _colorsCurrent[x];
			currentColor.r = startColor.r + delta * (goalColor.r - startColor.r);
			currentColor.g = startColor.g + delta * (goalColor.g - startColor.g);
			currentColor.b = startColor.b + delta * (goalColor.b - startColor.b);
			_currentPalette.setPixel(x, 0, { static_cast<sf::Uint8>(currentColor.r * 255.0f), static_cast<sf::Uint8>(currentColor.g * 255.0f), static_cast<sf::Uint8>(currentColor.b * 255.0f), static_cast<sf::Uint8>(currentColor.a * 255.0f) });
		}
		MarkForImageRendering();
		_colorTransitionTimer += Global::Clock::GetFrameTime().asSeconds();
	}
	ComputeImage();
	RenderImage();
}

void FractalSet::OnRender(Scene &scene)
{
	scene.Submit(_vertexArray);
}

void FractalSet::MarkForImageRendering() noexcept
{
	_reconstructImage = true;
}

void FractalSet::MarkForImageComputation() noexcept
{
	_recomputeImage = true;
}

void FractalSet::AddWorker(Worker *worker)
{
	worker->nWorkerComplete = &_nWorkerComplete;
	worker->fractalArray = _fractalArray;
	worker->simWidth = _simWidth;
	worker->alive = true;
	worker->thread = std::thread(&Worker::Compute, worker);
	_workers.push_back(worker);
}

void FractalSet::ResizeVertexArray(const sf::Vector2f &size)
{
	_vertexArrayDimensions = size;
	_shouldResize = true;
}

void FractalSet::SetSimBox(const Pair<sf::Vector2f, sf::Vector2f> &box)
{
	_simBox = box;
}

void FractalSet::SetComputeIterationCount(size_t iterations) noexcept
{
	_computeIterations = iterations;
}

void FractalSet::SetPalette(FractalSet::Palette palette) noexcept
{
	_desiredPalette = palette;
	_colorTransitionTimer = 0.0f;
	_colorsStart = _colorsCurrent;
}

void FractalSet::ComputeImage()
{
	if ( _recomputeImage )
	{
		if ( _shouldResize )
		{
			_simWidth = _vertexArrayDimensions.x;
			_simHeight = _vertexArrayDimensions.y;

			_vertexArray.resize(_simWidth * _simHeight);
			for ( size_t i = 0; i < _vertexArray.getVertexCount(); i++ )
			{
				_vertexArray[i].position = sf::Vector2f(static_cast<float>(std::floor(i % _simWidth)), static_cast<float>(std::floor(i / _simWidth)));
				_vertexArray[i].color = sf::Color{ 0, 0, 0, 255 };
			}

			//delete[] _fractalArray;
			_fractalArray = new int[_simWidth * _simHeight];
			for ( auto *worker : _workers )
			{
				worker->fractalArray = _fractalArray;
				worker->simWidth = _simWidth;
			}
			_shouldResize = false;
		}

		const double imageSectionWidth = static_cast<double>(_simWidth) / static_cast<double>(_workers.size());
		const double fractalSectionWidth = static_cast<double>(_simBox.second.x - _simBox.first.x) / static_cast<double>(_workers.size());
		_nWorkerComplete = 0;

		for ( size_t i = 0; i < _workers.size(); i++ )
		{
		    SE_CORE_INFO("Notifying worker: {}", i);
			_workers[i]->imageTL = sf::Vector2<double>(imageSectionWidth * i, 0.0f);
			_workers[i]->imageBR = sf::Vector2<double>(imageSectionWidth * static_cast<double>(i + 1), _simHeight);
			_workers[i]->fractalTL = sf::Vector2<double>(_simBox.first.x + static_cast<double>(fractalSectionWidth * i), _simBox.first.y);
			_workers[i]->fractalBR = sf::Vector2<double>(_simBox.first.x + fractalSectionWidth * static_cast<double>(i + 1), _simBox.second.y);
			_workers[i]->iterations = _computeIterations;

			std::unique_lock<std::mutex> lm(_workers[i]->mutex);
			_workers[i]->cvStart.notify_one();
		}

		int currentCollected = 0;
		while ( _nWorkerComplete < _workers.size() ) // Wait for all workers to complete
		{
		    if(currentCollected != _nWorkerComplete)
            {
                SE_CORE_INFO("Collected {} workers", _nWorkerComplete);
            }
            currentCollected = _nWorkerComplete;
		}
		_recomputeImage = false;
	}
}

void FractalSet::RenderImage()
{
	if ( _reconstructImage )
	{
		const auto *const colorPal = _currentPalette.getPixelsPtr();
		for ( int y = 0; y < _simHeight; y++ )
		{
			for ( int x = 0; x < _simWidth; x++ )
			{
				const int i = _fractalArray[y * _simWidth + x];
				const float offset = static_cast<float>(i) / static_cast<float>(_computeIterations) * static_cast<float>(PaletteWidth - 1);



				memcpy(&_vertexArray[y * _simWidth + x].color, &colorPal[static_cast<int>(offset) * 4], sizeof(sf::Uint8) * 3);
			}
		}
		_reconstructImage = false;
	}
}
}