#pragma once

#include "Common.h"
#include "Host.h"

namespace Se
{
struct Worker
{
	virtual ~Worker() = default;
	virtual void Compute() = 0;

	Atomic<size_t>* WorkerComplete = nullptr;

	int* FractalArray;
	int SimWidth = 0;

	Position ImageTL = {0.0, 0.0};
	Position ImageBR = {0.0, 0.0};
	Position FractalTL = {0.0, 0.0};
	Position FractalBR = {0.0, 0.0};

	size_t Iterations = 0;
	bool Alive = true;

	Thread Thread;
	ConditionVariable CvStart;
	Mutex Mutex;
};

class CpuHost : public Host
{
public:
	CpuHost(int simWidth, int simHeight);
	~CpuHost() override;

	void OnRender(Scene& scene) override;

	void AddWorker(Unique<Worker> worker);
	auto Workers() -> List<Unique<Worker>>&;
	auto Workers() const -> const List<Unique<Worker>>&;

private:
	void ComputeImage() override;
	void RenderImage() override;
	void Resize(int width, int height) override;

private:
	List<Unique<Worker>> _workers;
	Atomic<size_t> _nWorkerComplete = 0;

	sf::VertexArray _vertexArray;
	int* _fractalArray;
};
}
