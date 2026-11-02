#pragma once
#include <string>
#include <vector>
#include "Vertex.hpp"

class Framebuffer
{
private:
    const int HEIGHT, WIDTH, CENTER_X, CENTER_Y, K, Z_OFFSET;
    const std::string SHADES;

    // Default light pointing towards the camera
    float light_x = 0.0f;
    float light_y = 0.0f;
    float light_z = -1.0f;

    std::vector<char> buffer;
    std::vector<float> z_buffer;

    char calculate_vertex_char(const Vertex &v) const;

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

    // Read-only access for output and tests
    int width() const;
    int height() const;
    const std::vector<char> &chars() const;

    void clear();

    void render_vertex(const Vertex &v);

    void set_light_direction(float lx, float ly, float lz);
};