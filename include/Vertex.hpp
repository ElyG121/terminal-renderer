#pragma once

struct Vertex
{
    float x, y, z;
    float nx, ny, nz;

    // Default constructor
    Vertex() : x(0), y(0), z(0), nx(0), ny(0), nz(0) {}

    // Parameterized constructor
    Vertex(float x, float y, float z, float nx, float ny, float nz)
        : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz) {}

    // The new rotate method
    Vertex rotate(float cx, float sx, float cy, float sy, float cz, float sz) const;
};