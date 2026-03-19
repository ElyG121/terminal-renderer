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

    Vertex rot_v; // Cleanly scoped to this function now

    for (const Vertex &v : shape_ptr->vertices)
    {
        rotate_vertex(v, cx, sx, cy, sy, cz, sz, rot_v);

        rot_v.x += shape_ptr->world_x;
        rot_v.y += shape_ptr->world_y;
        rot_v.z += shape_ptr->world_z;

        screen.render_vertex(rot_v);
    }
}

void Renderer::rotate_vertex(const Vertex &v, float cx, float sx, float cy, float sy, float cz, float sz, Vertex &out)
{
    // --- POSITION ROTATION ---
    // Step 1: Rotate around X-axis
    float x1 = v.x;
    float y1 = v.y * cx - v.z * sx;
    float z1 = v.y * sx + v.z * cx;

    // Step 2: Rotate around Y-axis
    float x2 = x1 * cy + z1 * sy;
    float y2 = y1;
    float z2 = -x1 * sy + z1 * cy;

    // Step 3: Rotate around Z-axis
    out.x = x2 * cz - y2 * sz;
    out.y = x2 * sz + y2 * cz;
    out.z = z2;

    // --- NORMAL ROTATION ---
    // Step 1: Rotate around X-axis
    float nx1 = v.nx;
    float ny1 = v.ny * cx - v.nz * sx;
    float nz1 = v.ny * sx + v.nz * cx;

    // Step 2: Rotate around Y-axis
    float nx2 = nx1 * cy + nz1 * sy;
    float ny2 = ny1;
    float nz2 = -nx1 * sy + nz1 * cy;

    // Step 3: Rotate around Z-axis
    out.nx = nx2 * cz - ny2 * sz;
    out.ny = nx2 * sz + ny2 * cz;
    out.nz = nz2;
}

void Renderer::clear_screen()
{
    // Clear your internal framebuffer buffers here
    // screen.clear();
    screen.clear();
}

void Renderer::display()
{
    screen.display();
}