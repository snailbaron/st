#pragma once

#include <cmath>
#include <concepts>
#include <ostream>
#include <type_traits>

template <class T>
concept Coordinate = requires(T x, T y) {
    { x + y };
};

template <Coordinate T>
struct Vector {
    T x = 0;
    T y = 0;

    template <std::convertible_to<T> U>
    Vector& operator+=(const Vector<U>& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    template <std::convertible_to<T> U>
    Vector& operator-=(const Vector<U>& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    template <std::convertible_to<T> U>
    Vector& operator*=(U scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    template <std::convertible_to<T> U>
    Vector& operator/=(U scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    T sqLength() const
    {
        return x * x + y * y;
    }

    T length() const
    {
        return std::sqrt(sqLength());
    }
};

template <Coordinate U, Coordinate V>
Vector<std::common_type_t<U, V>> operator+(
    const Vector<U>& lhs, const Vector<V>& rhs)
{
    return {lhs.x + rhs.x, lhs.y + lhs.y};
}

template <Coordinate U, Coordinate V>
Vector<std::common_type_t<U, V>> operator-(
    const Vector<U>& lhs, const Vector<V>& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

template <Coordinate U, Coordinate V>
Vector<std::common_type_t<U, V>> operator*(Vector<U> vector, V scalar)
{
    return {vector.x * scalar, vector.y * scalar};
}

template <Coordinate U, Coordinate V>
Vector<std::common_type_t<U, V>> operator*(U scalar, Vector<V> vector)
{
    return {scalar * vector.x, scalar * vector.y};
}

template <Coordinate U, Coordinate V>
Vector<std::common_type_t<U, V>> operator/(Vector<U> vector, V scalar)
{
    return {vector.x / scalar, vector.y / scalar};
}

template <Coordinate T>
std::ostream& operator<<(std::ostream& output, const Vector<T>& vector)
{
    return output << "[" << vector.x << ", " << vector.y << "]";
}

template <Coordinate T>
struct Point {
    T x = 0;
    T y = 0;

    template <std::convertible_to<T> U>
    Point& operator+=(const Vector<U>& vector)
    {
        x += vector.x;
        y += vector.y;
        return *this;
    }

    template <std::convertible_to<T> U>
    Point& operator-=(const Vector<U>& vector)
    {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }

    Vector<T> vector() const
    {
        return {x, y};
    }
};

template <Coordinate U, Coordinate V>
Point<std::common_type_t<U, V>> operator+(
    Point<U> point, const Vector<V>& vector)
{
    return {point.x + vector.x, point.y + vector.y};
}

template <Coordinate U, Coordinate V>
Point<std::common_type_t<U, V>> operator-(
    Point<U> point, const Vector<V>& vector)
{
    return {point.x - vector.x, point.y - vector.y};
}

template <Coordinate U, Coordinate V>
Vector<std::common_type_t<U, V>> operator-(
    const Point<U>& lhs, const Point<V>& rhs)
{
    return {rhs.x - lhs.x, rhs.y - lhs.y};
}

template <Coordinate T>
std::ostream& operator<<(std::ostream& output, const Point<T>& point)
{
    return output << "(" << point.x << ", " << point.y << ")";
}

template <Coordinate T>
class Rect {
public:
    Rect() = default;

    Rect(float x, float y, float w, float h)
        : _corner{x, y}
        , _size{w, h}
    { }

    template <Coordinate U, Coordinate V>
    static Rect<std::common_type_t<U, V>> fromCenter(
        const Point<U>& center, const Vector<V>& size)
    {
        using T = std::common_type_t<U, V>;

        return Rect{
            center.x - size.x / 2,
            center.y - size.y / 2,
            static_cast<T>(size.x),
            static_cast<T>(size.y)
        };
    }

    template <std::convertible_to<T> U>
    Rect& operator+=(const Vector<U>& vector)
    {
        _corner += vector;
        return *this;
    }

    T minX() const
    {
        return _corner.x;
    }

    T maxX() const
    {
        return _corner.x + _size.x;
    }

    T minY() const
    {
        return _corner.y;
    }

    T maxY() const
    {
        return _corner.y + _size.y;
    }

    T width() const
    {
        return _size.x;
    }

    T height() const
    {
        return _size.y;
    }

    Point<T> center() const
    {
        return _corner + _size / 2;
    }

    const Point<T>& corner() const
    {
        return _corner;
    }

    void center(const Point<T>& center)
    {
        _corner = {center.x - _size.x / 2, center.y - _size.y / 2};
    }

    void size(Vector<T> size)
    {
        _size = std::move(size);
    }

    void shrink(T amount)
    {
        _corner += Vector<T>{amount, amount};
        _size -= 2 * Vector<T>{amount, amount};
    }

    [[nodiscard]] Rect shrinked(T amount) const
    {
        auto result = *this;
        result.shrink(amount);
        return result;
    }

private:
    Point<T> _corner;
    Vector<T> _size;
};

template <Coordinate U, Coordinate V>
Rect<std::common_type_t<U, V>> operator+(
    const Rect<U>& rect, const Vector<V>& vector)
{
    auto result = Rect<std::common_type_t<U, V>>{rect};
    result += vector;
    return result;
}

template <Coordinate U, Coordinate V>
std::common_type_t<U, V> distance(const Point<U>& lhs, const Point<V>& rhs)
{
    return (rhs - lhs).length();
}

template <Coordinate U, Coordinate V>
bool intersect(const Rect<U>& rect, const Point<V>& point)
{
    return point.x >= rect.minX() && point.x <= rect.maxX() &&
        point.y >= rect.minY() && point.y <= rect.maxY();
}

template <Coordinate U, Coordinate V>
bool intersect(const Point<U>& point, const Rect<V>& rect)
{
    return intersect(rect, point);
}