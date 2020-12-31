#pragma once

#include <chrono>

namespace Se
{
class FpsLimiter
{
public:
	static void Start();
	static void Adjust();

	static float GetDesiredFps() { return _desiredFps; }
	static void SetDesiredFps(float fps) { _desiredFps = fps; }

private:
	static float _desiredFps;
	static std::chrono::steady_clock::time_point _last;
};
}