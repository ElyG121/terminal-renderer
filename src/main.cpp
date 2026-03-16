#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <algorithm>
#include <memory> // For std::unique_ptr
#include "Vertex.h"
#include "Framebuffer.h"
#include "ShapeFactory.h"

void rotate_vertex(const Vertex &v, float cos_a, float sin_a, float cos_b, float sin_b, Vertex &out);

void render_scene(Framebuffer &screen, const std::vector<std::unique_ptr<Shape>> &scene, Vertex &rot_v);

void render_shape(const std::unique_ptr<Shape> &shape_ptr, float cos_a, float sin_a, float cos_b, float sin_b, Vertex &rot_v, Framebuffer &screen);

int main()
{
    std::vector<std::unique_ptr<Shape>> scene;

    // Create Torus and move it to the LEFT (-12 on the X axis)
    auto torus = ShapeFactory::createTorus(3.0f, 6.0f);
    torus->setPosition(-12.0f, 0.0f, 0.0f);
    scene.push_back(std::move(torus));

    // Create Cube and move it to the RIGHT (+12 on the X axis)
    auto cube = ShapeFactory::createCube(12.0f);
    cube->setPosition(12.0f, 0.0f, 0.0f);
    scene.push_back(std::move(cube));

    // Framebuffer(height, width, Z_OFFSET, K)
    Framebuffer screen(24, 80, 40, 35);
    std::cout << "\x1b[2J";
    Vertex rot_v;

    render_scene(screen, scene, rot_v);
    return 0;
}

void render_scene(Framebuffer &screen, const std::vector<std::unique_ptr<Shape>> &scene, Vertex &rot_v)
{
    while (true)
    {
        for (float angle = 0; angle <= 2 * M_PI; angle += 0.05)
        {
            screen.clear();

            float cos_a = cos(angle), sin_a = sin(angle);
            float cos_b = cos(angle * 0.5), sin_b = sin(angle * 0.5);

            // Loop through every shape in the scene
            for (const auto &shape_ptr : scene)
            {
                render_shape(shape_ptr, cos_a, sin_a, cos_b, sin_b, rot_v, screen);
            }

            screen.display();
            usleep(30000);
        }
    }
}

void render_shape(const std::unique_ptr<Shape> &shape_ptr, float cos_a, float sin_a, float cos_b, float sin_b, Vertex &rot_v, Framebuffer &screen)
{
    // Loop through every vertex in that shape
    for (const Vertex &v : shape_ptr->vertices)
    {
        // Rotate around the object's own local center
        rotate_vertex(v, cos_a, sin_a, cos_b, sin_b, rot_v);

        // Translate to the object's world position
        rot_v.x += shape_ptr->world_x;
        rot_v.y += shape_ptr->world_y;
        rot_v.z += shape_ptr->world_z;

        // Render to screen
        screen.render_vertex(rot_v);
    }
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
