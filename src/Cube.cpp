#include <cmath>
#include "Cube.hpp"

// The parameter now explicitly represents the total length of an edge
Cube::Cube(float edge_length)
{
    // Calculate the half-extent for centering around (0,0,0)
    float half = edge_length / 2.0f;
    const float step = 0.1f;
    int steps = static_cast<int>(edge_length / step + 0.5f);
    vertices.reserve((steps + 1) * (steps + 1) * 6);

    // Loop from -half to +half using integer steps to avoid floating-point accumulation
    for (int i = 0; i <= steps; ++i)
    {
        float u = -half + i * step;
        for (int j = 0; j <= steps; ++j)
        {
            float v = -half + j * step;
            // Front and Back
            vertices.push_back(Vertex(u, v, half, 0, 0, 1));
            vertices.push_back(Vertex(u, v, -half, 0, 0, -1));

            // Right and Left
            vertices.push_back(Vertex(half, u, v, 1, 0, 0));
            vertices.push_back(Vertex(-half, u, v, -1, 0, 0));

            // Top and Bottom
            vertices.push_back(Vertex(u, half, v, 0, 1, 0));
            vertices.push_back(Vertex(u, -half, v, 0, -1, 0));
        }
    }
}