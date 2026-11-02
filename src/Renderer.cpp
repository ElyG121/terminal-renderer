#include "Renderer.hpp"
#include "Mat3.hpp"

Renderer::Renderer(const ScreenSize &size, const Camera &camera)
    : screen(size, camera)
{
}

void Renderer::draw(const Scene &scene)
{
    screen.clear();

    for (const Entity &entity : scene)
    {
        render_entity(entity);
    }
}

const Framebuffer &Renderer::framebuffer() const
{
    return screen;
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
