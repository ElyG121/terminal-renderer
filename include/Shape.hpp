#pragma once
#include <vector>
#include <functional> // Required for lambdas
#include "Vertex.hpp"

class Shape
{
public:
    std::vector<Vertex> vertices;

    // 1. Position State
    float world_x = 0.0f, world_y = 0.0f, world_z = 0.0f;

    // 2. Rotation State
    float rot_x = 0.0f, rot_y = 0.0f, rot_z = 0.0f;

    // 3. Translation Velocity (Units per second)
    float vel_x = 0.0f, vel_y = 0.0f, vel_z = 0.0f;

    // 4. Angular Velocity (Radians per second)
    float rot_vel_x = 0.0f, rot_vel_y = 0.0f, rot_vel_z = 0.0f;

    // 5. The Behavior Injection (Lambda)
    std::function<void(Shape &, float)> update_behavior = nullptr;

    virtual ~Shape() = default;

    // Helper methods for easy setup
    void setPosition(float x, float y, float z)
    {
        world_x = x;
        world_y = y;
        world_z = z;
    }
    void setVelocity(float vx, float vy, float vz)
    {
        vel_x = vx;
        vel_y = vy;
        vel_z = vz;
    }
    void setAngularVelocity(float rx, float ry, float rz)
    {
        rot_vel_x = rx;
        rot_vel_y = ry;
        rot_vel_z = rz;
    }

    void update(float delta)
    {
        if (update_behavior)
        {
            update_behavior(*this, delta);
        }
        world_x += vel_x * delta;
        world_y += vel_y * delta;
        world_z += vel_z * delta;

        rot_x += rot_vel_x * delta;
        rot_y += rot_vel_y * delta;
        rot_z += rot_vel_z * delta;
    }
}; // End of Shape class