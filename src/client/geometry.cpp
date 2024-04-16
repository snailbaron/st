#include "geometry.hpp"

std::ostream& operator<<(std::ostream& output, const Point& point)
{
    return output << "(" << point.x << ", " << point.y << ")";
}
