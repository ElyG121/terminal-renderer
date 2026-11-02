#include <cmath>
#include "Mesh.hpp"

// M_PI is a POSIX extension, not standard C++ (std::numbers::pi requires C++20)
static constexpr float PI = 3.14159265358979323846f;

// The parameter represents the total length of an edge
Mesh make_cube(float edge_length)
{
    Mesh mesh;

    // Calculate the half-extent for centering around (0,0,0)
    float half = edge_length / 2.0f;
    const float step = 0.1f;
    int steps = static_cast<int>(edge_length / step + 0.5f);

    // Each face is a (steps + 1) x (steps + 1) grid, but faces share their edges.
    // Every edge point is kept on exactly one face: front/back keep all their points,
    // right/left skip the rows shared with front/back, and top/bottom skip both.
    // Total unique points: 6 * steps^2 + 2
    mesh.vertices.reserve(6 * steps * steps + 2);

    // Loop from -half to +half using integer steps to avoid floating-point accumulation
    for (int i = 0; i <= steps; ++i)
    {
        float u = -half + i * step;
        bool u_on_edge = (i == 0 || i == steps);

        for (int j = 0; j <= steps; ++j)
        {
            float v = -half + j * step;
            bool v_on_edge = (j == 0 || j == steps);

            // Front and Back (z = +/-half): every point
            mesh.vertices.push_back(Vertex{{u, v, half}, {0, 0, 1}});
            mesh.vertices.push_back(Vertex{{u, v, -half}, {0, 0, -1}});

            // Right and Left (x = +/-half): v is z, so its ends are on front/back
            if (!v_on_edge)
            {
                mesh.vertices.push_back(Vertex{{half, u, v}, {1, 0, 0}});
                mesh.vertices.push_back(Vertex{{-half, u, v}, {-1, 0, 0}});
            }

            // Top and Bottom (y = +/-half): u is x (ends on right/left), v is z (ends on front/back)
            if (!u_on_edge && !v_on_edge)
            {
                mesh.vertices.push_back(Vertex{{u, half, v}, {0, 1, 0}});
                mesh.vertices.push_back(Vertex{{u, -half, v}, {0, -1, 0}});
            }
        }
    }

    return mesh;
}

// R1 is the radius of the tube, R2 is the distance from the centre to the middle of the tube
Mesh make_torus(float R1, float R2)
{
    Mesh mesh;

    const float step = 0.05f;
    const int steps = static_cast<int>((2.0f * PI) / step + 0.5f);
    mesh.vertices.reserve(steps * steps);

    for (int ti = 0; ti < steps; ++ti)
    {
        float theta = ti * step;
        float costheta = cos(theta);
        float sintheta = sin(theta);

        for (int pi = 0; pi < steps; ++pi)
        {
            float phi = pi * step;
            float cosphi = cos(phi);
            float sinphi = sin(phi);

            float circle_x = R2 + R1 * costheta;
            float circle_y = R1 * sintheta;

            float x = circle_x * cosphi;
            float y = circle_y;
            float z = circle_x * sinphi;

            float nx = costheta * cosphi;
            float ny = sintheta;
            float nz = costheta * sinphi;

            mesh.vertices.push_back(Vertex{{x, y, z}, {nx, ny, nz}});
        }
    }

    return mesh;
}
