#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "Mesh.hpp"
#include "Vec3.hpp"

struct Transform
{
    Vec3 position; // World position
    Vec3 rotation; // Euler angles (radians)
};

struct Motion
{
    Vec3 velocity;         // Units per second
    Vec3 angular_velocity; // Radians per second
};

struct Entity
{
    std::shared_ptr<const Mesh> mesh; // Shared, read-only geometry
    Transform transform;
    Motion motion;
    std::function<void(Entity &, float)> behavior; // Optional custom physics

    void update(float delta);
};

using Scene = std::vector<Entity>;
