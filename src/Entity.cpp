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
