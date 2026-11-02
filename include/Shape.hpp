#pragma once
#include <vector>
#include <functional> // Required for lambdas
#include "Vec3.hpp"
#include "Vertex.hpp"

class Shape
{
public:
    std::vector<Vertex> vertices;

    Vec3 position;         // World position
    Vec3 rotation;         // Euler angles (radians)
    Vec3 velocity;         // Units per second
    Vec3 angular_velocity; // Radians per second

    // The Behavior Injection (Lambda)
    std::function<void(Shape &, float)> update_behavior = nullptr;

    virtual ~Shape() = default;

    // Helper methods for easy setup
    void setPosition(const Vec3 &p) { position = p; }
    void setVelocity(const Vec3 &v) { velocity = v; }
    void setAngularVelocity(const Vec3 &w) { angular_velocity = w; }

    void update(float delta)
    {
        if (update_behavior)
        {
            update_behavior(*this, delta);
        }
        position += velocity * delta;
        rotation += angular_velocity * delta;
    }
}; // End of Shape class
