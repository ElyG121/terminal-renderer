#include "Renderer.hpp"
#include "Entity.hpp"
#include "Mesh.hpp"
#include <memory>

int main()
{
    // 1. Initialize your new rendering engine
    Renderer engine(24, 80, 40, 25);
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

    engine.render_scene(scene);

    return 0;
}
