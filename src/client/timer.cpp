#include "timer.hpp"

#include <thread>

FrameTimer::FrameTimer(int fps)
    : _startTime{Clock::now()}
    , _delta{std::chrono::duration_cast<Clock::duration>(
        std::chrono::duration<double>{1.0 / fps})}
    , _deltaSeconds{1.f / fps}
{ }

int FrameTimer::operator()()
{
    auto now = Clock::now();
    auto currentFrame = (now - _startTime) / _delta;
    auto framesPassed = static_cast<int>(currentFrame - _lastFrame);
    _lastFrame = currentFrame;
    return framesPassed;
}

float FrameTimer::delta() const
{
    return _deltaSeconds;
}

void FrameTimer::relax()
{
    auto nextFrameTime = _startTime + _delta * (_lastFrame + 1);
    std::this_thread::sleep_until(nextFrameTime);
}
