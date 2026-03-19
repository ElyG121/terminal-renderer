#pragma once
#include <vector>
#include <memory>
#include "Shape.hpp"
#include "Framebuffer.hpp"

class Renderer
{
public:
    // Initialize the renderer and its internal framebuffer
    Renderer(int height, int width, int z_offset, int k);

    // The only public function the user needs to call
    void render_scene(std::vector<std::unique_ptr<Shape>> &scene);

    // Utility methods for the user to control the screen
    void clear_screen();
    void display();

private:
    Framebuffer screen; // The Renderer owns the screen now

    // Hidden internal rendering pipeline
    void render_shape(const std::unique_ptr<Shape> &shape_ptr);
    void rotate_vertex(const Vertex &v, float cx, float sx, float cy, float sy, float cz, float sz, Vertex &out);
};