#pragma once

#include <ostream>

struct Point {
    float x = 0.f;
    float y = 0.f;
};

std::ostream& operator<<(std::ostream& output, const Point& point);