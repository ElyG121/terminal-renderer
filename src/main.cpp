#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <algorithm>
#include "Vertex.h"
#include "Framebuffer.h"

void rotate_vertex(const Vertex &v, float cos_a, float sin_a, float cos_b, float sin_b, Vertex &out);
std::vector<Vertex> create_cube(int size);

int main()
{
    // Define shape
    std::vector<Vertex> shape = create_cube(7);

    // Initialize frame buffer
    Framebuffer screen(22, 80, 25, 15);

    // Clear screen
    std::cout << "\x1b[2J";

    Vertex rot_v;

    while (true)
    {
        for (float angle = 0; angle <= 2 * M_PI; angle += 0.05)
        {
            screen.clear();

            // Pre-calculate trig ONCE per frame
            float cos_a = cos(angle);
            float sin_a = sin(angle);
            float cos_b = cos(angle * 0.5);
            float sin_b = sin(angle * 0.5);

            for (const Vertex &v : shape)
            {
                rotate_vertex(v, cos_a, sin_a, cos_b, sin_b, rot_v);
                screen.render_vertex(rot_v);
            }

            screen.display();

            usleep(30000);
        }
    }
    return 0;
}

std::vector<Vertex> create_cube(int size)
{
    std::vector<Vertex> cube;
    int steps = (size * 2) * 10;
    cube.reserve(steps * steps * 6);

    for (float u = -size; u <= size; u += 0.1)
    {
        for (float v = -size; v <= size; v += 0.1)
        {
            cube.push_back(Vertex(u, v, size, 0, 0, 1));
            cube.push_back(Vertex(u, v, -size, 0, 0, -1));

            cube.push_back(Vertex(size, u, v, 1, 0, 0));
            cube.push_back(Vertex(-size, u, v, -1, 0, 0));

            cube.push_back(Vertex(u, size, v, 0, 1, 0));
            cube.push_back(Vertex(u, -size, v, 0, -1, 0));
        }
    }
    return cube;
}

void rotate_vertex(const Vertex &v, float cos_a, float sin_a, float cos_b, float sin_b, Vertex &out)
{

    // Rotate the point (x, y, z)
    out.x = v.x * cos_a - v.z * sin_a;
    out.y = -v.x * sin_a * sin_b + v.y * cos_b - v.z * cos_a * sin_b;
    out.z = v.x * sin_a * cos_b + v.y * sin_b + v.z * cos_a * cos_b;

    // Rotate the normal (nx, ny, nz)
    out.nx = v.nx * cos_a - v.nz * sin_a;
    out.ny = -v.nx * sin_a * sin_b + v.ny * cos_b - v.nz * cos_a * sin_b;
    out.nz = v.nx * sin_a * cos_b + v.ny * sin_b + v.nz * cos_a * cos_b;
}
