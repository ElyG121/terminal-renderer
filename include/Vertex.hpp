#pragma once
#include "Vec3.hpp"

// A point on a shape's surface and the direction that surface faces (used for lighting)
struct Vertex
{
    Vec3 position;
    Vec3 normal;
};
