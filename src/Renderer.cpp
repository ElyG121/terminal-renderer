#include "Renderer.hpp"
#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

Renderer::Renderer(int height, int width, int z_offset, int k)
    : screen(height, width, z_offset, k)
{
    std::cout << "\x1b[2J"; // Clear the terminal ONCE at startup
}

void Renderer::render_scene(std::vector<std::unique_ptr<Shape>> &scene, int max_frames)
{
    using clock = std::chrono::steady_clock;
    const auto frame_duration = std::chrono::milliseconds(33);
    auto previous = clock::now();
    int frame_count = 0;

    while (max_frames < 0 || frame_count < max_frames)
    {
        auto current = clock::now();
        std::chrono::duration<float> delta_duration = current - previous;
        previous = current;
        float delta = delta_duration.count();

        clear_screen();

        for (const auto &shape : scene)
        {
            shape->update(delta);
            render_shape(shape);
        }

        display();

        auto elapsed = clock::now() - current;
        if (elapsed < frame_duration)
        {
            std::this_thread::sleep_for(frame_duration - elapsed);
        }

        frame_count++;
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