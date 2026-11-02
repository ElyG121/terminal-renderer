#include "Renderer.hpp"
#include "Mat3.hpp"
#include <iostream>
#include <chrono>
#include <thread>

Renderer::Renderer(const ScreenSize &size, const Camera &camera)
    : screen(size, camera), terminal(std::cout)
{
    terminal.begin(); // Clear the terminal ONCE at startup
}

void Renderer::render_scene(Scene &scene, int max_frames)
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

        for (Entity &entity : scene)
        {
            entity.update(delta);
            render_entity(entity);
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

void Renderer::render_entity(const Entity &entity)
{
    // An entity without geometry has nothing to draw
    if (!entity.mesh)
        return;

    // Build the rotation once per entity, not once per vertex
    const Mat3 rotation = Mat3::rotation(entity.transform.rotation);

    for (const Vertex &v : entity.mesh->vertices)
    {
        // Rotate around the shape's centre, then move it into the world.
        // Normals are only rotated: they are directions, not positions.
        Vertex world_v{rotation * v.position + entity.transform.position, rotation * v.normal};

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