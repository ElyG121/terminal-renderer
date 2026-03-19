#include <cmath>
#include "Torus.hpp"

Torus::Torus(float R1, float R2)
{
    float step = 0.05f;
    vertices.reserve(16000);

    for (float theta = 0; theta < 2 * M_PI; theta += step)
    {
        float costheta = cos(theta);
        float sintheta = sin(theta);

        for (float phi = 0; phi < 2 * M_PI; phi += step)
        {
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