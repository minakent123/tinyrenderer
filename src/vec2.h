#pragma once

#include <cmath>

template<typename T>
struct Vec2 final
{
    Vec2()
        : x(0)
        , y(0)
    {}

    explicit Vec2(T a)
        : x(a)
        , y(a)
    {}

    Vec2(T nx, T ny)
        : x(nx)
        , y(ny)
    {}

    Vec2(const Vec2& v)
        : x(v.x)
        , y(v.y)
    {}

    Vec2& operator=(const Vec2& p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }

    bool operator==(const Vec2& v) const
    {
        return x == v.x && y == v.y;
    }

    bool operator!=(const Vec2& v) const
    {
        return x != v.x || y != v.y;
    }

    float MagnitudeSquared() const
    {
        return x * x + y * y;
    }

    float Magnitude() const
    {
        return sqrt(MagnitudeSquared());
    }

    Vec2 operator-() const
    {
        return Vec2(-x, -y);
    }

    Vec2 operator+(const Vec2& v) const
    {
        return Vec2(x + v.x, y + v.y);
    }

    Vec2 operator-(const Vec2& v) const
    {
        return Vec2(x - v.x, y - v.y);
    }

    Vec2 operator*(float f) const
    {
        return Vec2(static_cast<T>(x * f), static_cast<T>(y * f));
    }

    Vec2 operator/(float f) const
    {
        f = 1.0f / f;
        return Vec2(static_cast<T>(x * f), static_cast<T>(y * f));
    }

    Vec2& operator+=(const Vec2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2& operator*=(float f)
    {
        x *= f;
        y *= f;
        return *this;
    }

    Vec2& operator/=(float f)
    {
        f = 1.0f / f;
        x *= f;
        y *= f;
        return *this;
    }

    T Dot(const Vec2& v) const
    {
        return x * v.x + y * v.y;
    }

    float Normalize()
    {
        const float m = Magnitude();
        if (m > 0.0f)
            *this /= m;
        return m;
    }

    Vec2 Multiply(const Vec2& a) const
    {
        return Vec2(x * a.x, y * a.y);
    }

    Vec2 Abs() const
    {
        return Vec2(std::abs(x), std::abs(y));
    }

    T x;
    T y;
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int32_t>;
