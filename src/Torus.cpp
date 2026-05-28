#include <cmath>
#include "Torus.hpp"

Torus::Torus(float R1, float R2)
{
    const float step = 0.05f;
    const int steps = static_cast<int>((2.0f * M_PI) / step + 0.5f);
    vertices.reserve(steps * steps);

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

            vertices.push_back(Vertex(x, y, z, nx, ny, nz));
        }
    }
}