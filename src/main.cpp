#include "Renderer.hpp"
#include "Entity.hpp"
#include "FixedTimestep.hpp"
#include "Mesh.hpp"
#include "TerminalOutput.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

int main()
{
    // 1. Initialize your new rendering engine
    // Default camera: distance 40, focal length 25 (see Camera.hpp)
    Renderer engine(ScreenSize{80, 24}, Camera{});
    engine.set_light_direction({0.7f, -1.0f, -0.5f});
    Scene scene;

    // 2. Setup Scene
    Entity torus;
    torus.mesh = std::make_shared<const Mesh>(make_torus(2.5f, 5.0f));
    torus.transform.position = {-14.0f, 0.0f, 0.0f};
    torus.motion.angular_velocity = {1.0f, 0.5f, 0.5f};
    scene.push_back(std::move(torus));

    Entity cube;
    cube.mesh = std::make_shared<const Mesh>(make_cube(9.0f));
    cube.transform.position = {14.0f, -8.0f, 0.0f};
    cube.motion.angular_velocity = {0.5f, 1.0f, 0.0f};

    // Gravity plus a bounce that loses 15% of its speed
    cube.behavior = [](Entity &e, float d)
    {
        e.motion.velocity.y += 15.0f * d;
        if (e.transform.position.y > 8.0f)
        {
            e.transform.position.y = 8.0f;
            e.motion.velocity.y = e.motion.velocity.y * -0.85f;
        }
    };
    scene.push_back(std::move(cube));

    // 3. Main loop: main owns time and output, the renderer only draws
    TerminalOutput terminal(std::cout);
    terminal.begin(); // Clear the terminal ONCE at startup

    FixedTimestep timestep(1.0f / 60.0f);                      // Physics at 60 Hz
    const auto frame_duration = std::chrono::milliseconds(33); // Drawing at ~30 FPS

    using clock = std::chrono::steady_clock;
    auto previous = clock::now();

    while (true)
    {
        auto frame_start = clock::now();
        float frame_seconds = std::chrono::duration<float>(frame_start - previous).count();
        previous = frame_start;

        // Physics always moves in equal steps, however long the frame took
        timestep.advance(frame_seconds, [&](float dt) { update_scene(scene, dt); });

        engine.draw(scene);
        terminal.present(engine.framebuffer());

        // Sleep for whatever is left of this frame's time budget
        auto elapsed = clock::now() - frame_start;
        if (elapsed < frame_duration)
        {
            std::this_thread::sleep_for(frame_duration - elapsed);
        }
    }

    return 0;
}
