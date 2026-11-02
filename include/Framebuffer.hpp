#pragma once
#include <string>
#include <vector>
#include "Camera.hpp"
#include "Vec3.hpp"
#include "Vertex.hpp"

class Framebuffer
{
private:
    const int HEIGHT, WIDTH, CENTER_X, CENTER_Y;
    const Camera camera;
    const std::string SHADES;

    // Default light pointing towards the camera
    Vec3 light{0.0f, 0.0f, -1.0f};

    std::vector<char> buffer;
    std::vector<float> z_buffer;

    char calculate_vertex_char(const Vertex &v) const;

public:
    /* Constructor */
    Framebuffer(const ScreenSize &size, const Camera &camera)
        : HEIGHT(size.height), WIDTH(size.width),
          CENTER_X(size.width / 2), CENTER_Y(size.height / 2), camera(camera),
          SHADES(".,-~:;=!*#$@"),
          buffer(size.height * size.width, ' '),
          z_buffer(size.height * size.width, 0.0f)
    {
    }

    // Read-only access for output and tests
    int width() const;
    int height() const;
    const std::vector<char> &chars() const;

    void clear();

    void render_vertex(const Vertex &v);

    void set_light_direction(const Vec3 &direction);
};