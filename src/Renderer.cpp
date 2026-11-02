#include "Renderer.hpp"
#include "Mat3.hpp"
#include <iostream>
#include <chrono>
#include <thread>

Renderer::Renderer(int height, int width, int z_offset, int k)
    : screen(height, width, z_offset, k), terminal(std::cout)
{
    terminal.begin(); // Clear the terminal ONCE at startup
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
    // Build the rotation once per shape, not once per vertex
    const Mat3 rotation = Mat3::rotation(shape_ptr->rotation);

    for (const Vertex &v : shape_ptr->vertices)
    {
        // Rotate around the shape's centre, then move it into the world.
        // Normals are only rotated: they are directions, not positions.
        Vertex world_v{rotation * v.position + shape_ptr->position, rotation * v.normal};

        screen.render_vertex(world_v);
    }
}

void Renderer::set_light_direction(const Vec3 &direction)
{
    // Delegate the math to the internal framebuffer
    screen.set_light_direction(direction);
}

void Renderer::clear_screen()
{
    screen.clear();
}

void Renderer::display()
{
    terminal.present(screen);
}