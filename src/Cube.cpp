#include <cmath>
#include "Cube.h"

// The parameter now explicitly represents the total length of an edge
Cube::Cube(float edge_length)
{
    // Calculate the half-extent for centering around (0,0,0)
    float half = edge_length / 2.0f;

    // Number of steps is the full edge length divided by the 0.1 step size
    int steps = edge_length * 10;
    vertices.reserve(steps * steps * 6);

    // Loop from -half to +half
    for (float u = -half; u <= half; u += 0.1f)
    {
        for (float v = -half; v <= half; v += 0.1f)
        {
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