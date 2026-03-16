#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Vertex.h"

class Framebuffer
{
private:
    const int HEIGHT, WIDTH, CENTER_X, CENTER_Y, K, Z_OFFSET;
    const std::string SHADES;

    std::vector<char> buffer;
    std::vector<float> z_buffer;

public:
    /* Constructor */
    Framebuffer(const int height, const int width, const int z_offset, const int k)
        : HEIGHT(height), WIDTH(width),
          CENTER_X(width / 2), CENTER_Y(height / 2), K(k), Z_OFFSET(z_offset),
          SHADES(".,-~:;=!*#$@"),
          buffer(height * width, ' '),
          z_buffer(height * width, 0.0f)
    {
    }

    void clear();

    void render_vertex(const Vertex &v);

    void display() const;
};