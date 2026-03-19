#include "Renderer.hpp"
#include <iostream>
#include <cmath>
#include <unistd.h>

Renderer::Renderer(int height, int width, int z_offset, int k)
    : screen(height, width, z_offset, k)
{
    std::cout << "\x1b[2J"; // Clear the terminal ONCE at startup
}

void Renderer::render_scene(std::vector<std::unique_ptr<Shape>> &scene)
{
    float delta = 0.03f;
    while (true)
    {
        clear_screen();

        for (const auto &shape : scene)
        {
            shape->update(delta);
            render_shape(shape);
        }

        display();
        usleep(30000);
    }
}

void Renderer::render_shape(const std::unique_ptr<Shape> &shape_ptr)
{
    float cx = cos(shape_ptr->rot_x);
    float sx = sin(shape_ptr->rot_x);
    float cy = cos(shape_ptr->rot_y);
    float sy = sin(shape_ptr->rot_y);
    float cz = cos(shape_ptr->rot_z);
    float sz = sin(shape_ptr->rot_z);

    for (const Vertex &v : shape_ptr->vertices)
    {
        // 1. The vertex handles its own rotation!
        Vertex rot_v = v.rotate(cx, sx, cy, sy, cz, sz);

        // 2. The renderer handles world translation
        rot_v.x += shape_ptr->world_x;
        rot_v.y += shape_ptr->world_y;
        rot_v.z += shape_ptr->world_z;

        // 3. The renderer draws to the screen
        screen.render_vertex(rot_v);
    }
}

void Renderer::set_light_direction(float lx, float ly, float lz)
{
    // Delegate the math to the internal framebuffer
    screen.set_light_direction(lx, ly, lz);
}

void Renderer::clear_screen()
{
    screen.clear();
}

void Renderer::display()
{
    screen.display();
}