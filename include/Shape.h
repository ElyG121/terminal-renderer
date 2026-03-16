#pragma once
#include <vector>
#include "Vertex.h"

class Shape
{
public:
    std::vector<Vertex> vertices;

    float world_x = 0.0f;
    float world_y = 0.0f;
    float world_z = 0.0f;

    // A virtual destructor is MANDATORY in C++ polymorphism
    // to ensure derived classes clean up their memory properly.
    virtual ~Shape() = default;

    void setPosition(float x, float y, float z)
    {
        world_x = x;
        world_y = y;
        world_z = z;
    }
};