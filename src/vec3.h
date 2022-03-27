#pragma once

#include <cmath>

struct Vec3 final
{
    Vec3()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
    {}

    explicit Vec3(float a)
        : x(a)
        , y(a)
        , z(a)
    {}

    Vec3(float nx, float ny, float nz)
        : x(nx)
        , y(ny)
        , z(nz)
    {}

    Vec3(const Vec3& v)
        : x(v.x)
        , y(v.y)
        , z(v.z)
    {}

    Vec3& operator=(const Vec3& p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
        return *this;
    }

    bool operator==(const Vec3& v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    bool operator!=(const Vec3& v) const
    {
        return x != v.x || y != v.y || z != v.z;
    }

    float MagnitudeSquared() const
    {
        return x * x + y * y + z * z;
    }

    float Magnitude() const
    {
        return sqrt(MagnitudeSquared());
    }

    Vec3 operator-() const
    {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator+(const Vec3& v) const
    {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }

    Vec3 operator-(const Vec3& v) const
    {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    Vec3 operator*(float f) const
    {
        return Vec3(x * f, y * f, z * f);
    }

    Vec3 operator/(float f) const
    {
        f = 1.0f / f;
        return Vec3(x * f, y * f, z * f);
    }

    Vec3& operator+=(const Vec3& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vec3& operator*=(float f)
    {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    Vec3& operator/=(float f)
    {
        f = 1.0f / f;
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }

    float Dot(const Vec3& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec3 Cross(const Vec3& v) const
    {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    float Normalize()
    {
        const float m = Magnitude();
        if (m > 0.0f)
            *this /= m;
        return m;
    }

    Vec3 Multiply(const Vec3& a) const
    {
        return Vec3(x * a.x, y * a.y, z * a.z);
    }

    Vec3 Abs() const
    {
        return Vec3(std::abs(x), std::abs(y), std::abs(z));
    }

    float x;
    float y;
    float z;
};
