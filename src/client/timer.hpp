#pragma once

#include <chrono>

class FrameTimer {
public:
    FrameTimer(int fps);
    int operator()();
    float delta() const;
    void relax();

private:
    using Clock = std::chrono::high_resolution_clock;

    Clock::time_point _startTime;
    Clock::duration _delta;
    float _deltaSeconds = 0.f;
    size_t _lastFrame = 0;
};