#pragma once

#include <limits>

struct ScreenCoordinate {
    float pixels = 0.f;
    float fraction = 0.f;
};

consteval ScreenCoordinate operator""_px(long double pixels)
{
    return ScreenCoordinate{.pixels = static_cast<float>(pixels)};
}

consteval ScreenCoordinate operator""_fr(long double fraction)
{
    return ScreenCoordinate{.fraction = static_cast<float>(fraction)};
}