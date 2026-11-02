#pragma once
#include <cmath>

// A 3D vector used for positions, directions, normals and Euler angles.
struct Vec3
{
    float x = 0.0f, y = 0.0f, z = 0.0f;

    Vec3 &operator+=(const Vec3 &o)
    {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }
};

inline Vec3 operator+(const Vec3 &a, const Vec3 &b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 operator-(const Vec3 &a, const Vec3 &b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 operator*(const Vec3 &v, float s) { return {v.x * s, v.y * s, v.z * s}; }
inline Vec3 operator*(float s, const Vec3 &v) { return v * s; }

inline float dot(const Vec3 &a, const Vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline float length(const Vec3 &v) { return std::sqrt(dot(v, v)); }

// Returns a unit-length copy of v, or v unchanged if it has zero length
inline Vec3 normalized(const Vec3 &v)
{
    float len = length(v);
    return len != 0.0f ? v * (1.0f / len) : v;
}
