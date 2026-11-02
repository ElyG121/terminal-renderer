#include "Entity.hpp"

void Entity::update(float delta)
{
    if (behavior)
    {
        behavior(*this, delta);
    }
    transform.position += motion.velocity * delta;
    transform.rotation += motion.angular_velocity * delta;
}

void update_scene(Scene &scene, float delta)
{
    for (Entity &entity : scene)
    {
        entity.update(delta);
    }
}
