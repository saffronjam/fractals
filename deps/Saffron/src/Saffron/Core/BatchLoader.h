#pragma once

#include "Saffron/Base.h"

namespace Se
{
class BatchLoader : public Signaller
{
public:
	struct Signals
	{
		static SignalAggregate<void> OnStart;
		static SignalAggregate<void> OnFinish;
	};

public:
	explicit BatchLoader(String name);
	~BatchLoader();

	void Submit(Function<void()> function, String shortDescription);
	void Execute();
	void ForceExit();
	void Reset();

	float GetProgress() const { return _progress; }
	const String *GetStatus() const { return _status; }
	size_t GetNoJobs() const { return _queue.size(); }
	size_t GetNoJobsDone() const { return _noJobsDone; }
	size_t GetNoJobsLeft() const { return GetNoJobs() - _noJobsDone; }

	bool IsFinished() const { return _progress >= 100.0f; }

	Mutex &GetExecutionMutex() { return _executionMutex; }

private:
	String _name;

	ArrayList<Pair<Function<void()>, String>> _queue;
	size_t _noJobsDone = 0;

	Atomic<float> _progress = 0.0f;
	Atomic<const String *> _status = nullptr;
	Atomic<bool> _running = false, _shouldExit = false;
	Mutex _queueMutex, _executionMutex;
	Thread _worker;
};
}

