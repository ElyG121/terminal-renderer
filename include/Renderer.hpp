#pragma once
#include "Camera.hpp"
#include "Entity.hpp"
#include "Framebuffer.hpp"

// Draws a scene into a framebuffer, one frame per call.
// It does not own the main loop, time, or output: the caller decides those.
class Renderer
{
public:
    // Initialize the renderer and its internal framebuffer
    Renderer(const ScreenSize &size, const Camera &camera = Camera{});

    void set_light_direction(const Vec3 &direction);

    // Clear the framebuffer and draw every entity. Does not change the scene.
    void draw(const Scene &scene);

    // The finished frame, for output (e.g. TerminalOutput::present) or tests
    const Framebuffer &framebuffer() const;

private:
    Framebuffer screen; // The Renderer owns the screen

    // Hidden internal rendering pipeline
    void render_entity(const Entity &entity);
};
