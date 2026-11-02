#pragma once
#include "Camera.hpp"
#include "Entity.hpp"
#include "Framebuffer.hpp"
#include "TerminalOutput.hpp"

class Renderer
{
public:
    // Initialize the renderer and its internal framebuffer
    Renderer(const ScreenSize &size, const Camera &camera = Camera{});

    // The only public function the user needs to call
    void render_scene(Scene &scene, int max_frames = -1);

    void set_light_direction(const Vec3 &direction);

    // Utility methods for the user to control the screen
    void clear_screen();
    void display();

private:
    Framebuffer screen; // The Renderer owns the screen now
    TerminalOutput terminal; // Writes the screen to the terminal (declared after screen: members init in this order)

    // Hidden internal rendering pipeline
    void render_entity(const Entity &entity);
};