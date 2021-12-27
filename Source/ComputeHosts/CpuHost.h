#pragma once

#include "Common.h"
#include "Host.h"

namespace Se
{
struct Worker
{
	virtual ~Worker() = default;
	virtual void Compute() = 0;

	std::atomic<size_t>* WorkerComplete = nullptr;

	int* FractalArray;
	int SimWidth = 0;

	Position ImageTL = {0.0, 0.0};
	Position ImageBR = {0.0, 0.0};
	Position FractalTL = {0.0, 0.0};
	Position FractalBR = {0.0, 0.0};

	size_t Iterations = 0;
	bool Alive = true;

	std::thread Thread;
	std::condition_variable CvStart;
	std::mutex Mutex;
};

class CpuHost : public Host
{
public:
	CpuHost(int simWidth, int simHeight);
	~CpuHost() override;

	void OnRender(Scene& scene) override;

	void AddWorker(std::unique_ptr<Worker> worker);
	auto Workers() -> std::vector<std::unique_ptr<Worker>>&;
	auto Workers() const -> const std::vector<std::unique_ptr<Worker>>&;

private:
	void ComputeImage() override;
	void RenderImage() override;
	void Resize(int width, int height) override;

private:
	std::vector<std::unique_ptr<Worker>> _workers;
	std::atomic<size_t> _nWorkerComplete = 0;

	sf::VertexArray _vertexArray;
	int* _fractalArray;
};
}
