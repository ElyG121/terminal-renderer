#include "Renderer.hpp"
#include "ShapeFactory.hpp"
#include <vector>

int main()
{
    // 1. Initialize your new rendering engine
    Renderer engine(24, 80, 40, 25);
    engine.set_light_direction({0.7f, -1.0f, -0.5f});
    std::vector<std::unique_ptr<Shape>> scene;

    // 2. Setup Scene
    auto torus = ShapeFactory::createTorus(2.5f, 5.0f);
    torus->setPosition({-14.0f, 0.0f, 0.0f});
    torus->setAngularVelocity({1.0f, 0.5f, 0.5f});
    scene.push_back(std::move(torus));

    auto cube = ShapeFactory::createCube(9.0f);
    cube->setPosition({14.0f, -8.0f, 0.0f});
    cube->setAngularVelocity({0.5f, 1.0f, 0.0f});

    cube->update_behavior = [](Shape &s, float d)
    {
        s.velocity.y += 15.0f * d;
        if (s.position.y > 8.0f)
        {
            s.position.y = 8.0f;
            s.velocity.y = s.velocity.y * -0.85f;
        }
    };
    scene.push_back(std::move(cube));

    engine.render_scene(scene);

    return 0;
}