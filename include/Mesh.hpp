#pragma once
#include <vector>
#include "Vertex.hpp"

// The geometry of a shape: points on its surface with normals, centred on (0,0,0).
// Many entities can share one Mesh.
struct Mesh
{
    std::vector<Vertex> vertices;
};

Mesh make_cube(float edge_length);
Mesh make_torus(float R1, float R2);
