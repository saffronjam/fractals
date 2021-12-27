#include "ComputeHosts/CpuHost.h"

#include "PaletteManager.h"

namespace Se
{
CpuHost::CpuHost(int simWidth, int simHeight) :
	Host(HostType::Cpu, "CPU", simWidth, simHeight),
	_vertexArray(sf::PrimitiveType::Points, SimWidth() * SimHeight()),
	_fractalArray(new int[SimWidth() * SimHeight()])
{
	for (size_t i = 0; i < _vertexArray.getVertexCount(); i++)
	{
		_vertexArray[i].position = sf::Vector2f(static_cast<float>(std::floor(i % SimWidth())),
		                                        static_cast<float>(std::floor(i / SimWidth())));
		_vertexArray[i].color = sf::Color{0, 0, 0, 255};
	}
}

CpuHost::~CpuHost()
{
	for (auto& worker : _workers)
	{
		worker->Alive = false;
		worker->CvStart.notify_all();
		if (worker->Thread.joinable()) worker->Thread.join();
	}
	_workers.clear();
}

void CpuHost::OnRender(Scene& scene)
{
	scene.ActivateScreenSpaceDrawing();
	scene.Submit(_vertexArray);
	scene.DeactivateScreenSpaceDrawing();
}

void CpuHost::AddWorker(std::unique_ptr<Worker> worker)
{
	worker->WorkerComplete = &_nWorkerComplete;
	worker->FractalArray = _fractalArray;
	worker->SimWidth = SimWidth();
	worker->Alive = true;
	worker->Thread = std::thread(&Worker::Compute, &*worker);
	_workers.emplace_back(std::move(worker));
}

auto CpuHost::Workers() -> std::vector<std::unique_ptr<Worker>>&
{
	return _workers;
}

auto CpuHost::Workers() const -> const std::vector<std::unique_ptr<Worker>>&
{
	return const_cast<CpuHost&>(*this).Workers();
}

void CpuHost::ComputeImage()
{
	const double imageSectionWidth = static_cast<double>(SimWidth()) / static_cast<double>(_workers.size());
	const double fractalSectionWidth = (SimBox().BottomRight.x - SimBox().TopLeft.x) / static_cast<double>(_workers.
		size());

	_nWorkerComplete = 0;

	const auto simBox = SimBox();
	const auto tl = simBox.TopLeft;
	const auto br = simBox.BottomRight;
	const auto simHeight = SimHeight();
	const auto iterations = ComputeIterations();

	for (size_t i = 0; i < _workers.size(); i++)
	{
		_workers[i]->ImageTL = sf::Vector2(imageSectionWidth * i, 0.0);
		_workers[i]->ImageBR = sf::Vector2<double>(imageSectionWidth * static_cast<double>(i + 1), simHeight);
		_workers[i]->FractalTL = sf::Vector2(tl.x + i * fractalSectionWidth, tl.y);
		_workers[i]->FractalBR = sf::Vector2(tl.x + fractalSectionWidth * static_cast<double>(i + 1), br.y);
		_workers[i]->Iterations = iterations;

		std::unique_lock lm(_workers[i]->Mutex);
		_workers[i]->CvStart.notify_one();
	}

	while (_nWorkerComplete < _workers.size()) // Wait for all workers to complete
	{
	}
}

void CpuHost::RenderImage()
{
	const auto* const colorPal = PaletteManager::Instance().DesiredPixelPtr();

	const auto simWidth = SimWidth();
	const auto simHeight = SimHeight();
	const auto iterations = ComputeIterations();

	for (int y = 0; y < simHeight; y++)
	{
		for (int x = 0; x < simWidth; x++)
		{
			const int i = _fractalArray[y * simWidth + x];
			const float offset = static_cast<float>(i) / static_cast<float>(iterations) * static_cast<float>(
				PaletteManager::PaletteWidth - 1);


			memcpy(&_vertexArray[y * simWidth + x].color, &colorPal[static_cast<int>(offset) * 4],
			       sizeof(sf::Uint8) * 3);
		}
	}
}

void CpuHost::Resize(int width, int height)
{
	_vertexArray.resize(width * height);
	for (size_t i = 0; i < _vertexArray.getVertexCount(); i++)
	{
		_vertexArray[i].position = sf::Vector2f(static_cast<float>(std::floor(i % width)),
		                                        static_cast<float>(std::floor(i / width)));
		_vertexArray[i].color = sf::Color{0, 0, 0, 255};
	}

	//delete[] _fractalArray;
	_fractalArray = new int[width * height];
	for (const auto& worker : _workers)
	{
		worker->FractalArray = _fractalArray;
		worker->SimWidth = width;
	}
}
}
