#pragma once
#include <cmath>
#include "Vec3.hpp"

// A 3x3 matrix, used for rotations. m[row][col].
struct Mat3
{
    float m[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; // Identity by default

    Vec3 operator*(const Vec3 &v) const
    {
        return {m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z};
    }

    Mat3 operator*(const Mat3 &o) const
    {
        Mat3 r;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                r.m[i][j] = m[i][0] * o.m[0][j] + m[i][1] * o.m[1][j] + m[i][2] * o.m[2][j];
        return r;
    }

    // Rotation by Euler angles (radians), applied in X, then Y, then Z order.
    // Build this once per shape per frame, then apply it to every vertex.
    static Mat3 rotation(const Vec3 &euler)
    {
        float cx = std::cos(euler.x), sx = std::sin(euler.x);
        float cy = std::cos(euler.y), sy = std::sin(euler.y);
        float cz = std::cos(euler.z), sz = std::sin(euler.z);

        Mat3 rx, ry, rz;
        rx.m[1][1] = cx; rx.m[1][2] = -sx;
        rx.m[2][1] = sx; rx.m[2][2] = cx;

        ry.m[0][0] = cy;  ry.m[0][2] = sy;
        ry.m[2][0] = -sy; ry.m[2][2] = cy;

        rz.m[0][0] = cz; rz.m[0][1] = -sz;
        rz.m[1][0] = sz; rz.m[1][1] = cz;

        // The rightmost matrix is applied first, so X happens first
        return rz * ry * rx;
    }
};
