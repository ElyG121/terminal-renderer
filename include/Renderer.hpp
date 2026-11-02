#pragma once
#include <vector>
#include <memory>
#include "Shape.hpp"
#include "Framebuffer.hpp"
#include "TerminalOutput.hpp"

class Renderer
{
public:
    // Initialize the renderer and its internal framebuffer
    Renderer(int height, int width, int z_offset, int k);

    // The only public function the user needs to call
    void render_scene(std::vector<std::unique_ptr<Shape>> &scene, int max_frames = -1);

    void set_light_direction(float lx, float ly, float lz);

    // Utility methods for the user to control the screen
    void clear_screen();
    void display();

private:
    Framebuffer screen; // The Renderer owns the screen now
    TerminalOutput terminal; // Writes the screen to the terminal (declared after screen: members init in this order)

    // Hidden internal rendering pipeline
    void render_shape(const std::unique_ptr<Shape> &shape_ptr);
};