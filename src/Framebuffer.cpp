#include "Framebuffer.hpp"
#include <algorithm>

int Framebuffer::width() const
{
    return WIDTH;
}

int Framebuffer::height() const
{
    return HEIGHT;
}

const std::vector<char> &Framebuffer::chars() const
{
    return buffer;
}

void Framebuffer::set_light_direction(const Vec3 &direction)
{
    // Normalize the vector so the dot product stays within the [-1, 1] range.
    // A zero vector has no direction, so keep the current light.
    if (length(direction) != 0.0f)
    {
        light = normalized(direction);
    }
}

void Framebuffer::clear()
{
    // Reset terminal buffers
    std::fill(buffer.begin(), buffer.end(), ' ');
    std::fill(z_buffer.begin(), z_buffer.end(), 0.0f);
}

void Framebuffer::render_vertex(const Vertex &v)
{
    // --- A. NEAR-PLANE CLIPPING ---
    // Distance in front of the camera. At or behind the camera this is <= 0, and
    // dividing by it would give infinite or flipped screen coordinates.
    float depth = v.position.z + camera.distance;
    if (depth < camera.near_plane)
        return;

    // --- B. PROJECTION MATH ---
    float rot_ooz = 1.0f / depth;
    int screen_x = (int)(CENTER_X + (camera.focal_length * v.position.x * camera.char_aspect * rot_ooz) + 0.5f);
    int screen_y = (int)(CENTER_Y + (camera.focal_length * v.position.y * rot_ooz) + 0.5f);

    int index = screen_y * WIDTH + screen_x;

    // Bounds check
    if (screen_x >= 0 && screen_x < WIDTH && screen_y >= 0 && screen_y < HEIGHT)
    {
        // --- C. Z-BUFFER CHECK ---
        if (rot_ooz > z_buffer[index])
        {
            // --- D. DRAW TO BUFFERS ---
            z_buffer[index] = rot_ooz;
            buffer[index] = calculate_vertex_char(v);
        }
    }
}

char Framebuffer::calculate_vertex_char(const Vertex &v) const
{
    // Calculate the dot product of the vertex normal and the light vector
    float luminance = dot(v.normal, light);

    if (luminance < 0)
        luminance = 0; // Clamp negative light to 0
    if (luminance > 1)
        luminance = 1; // Clamp light to 1

    // Look up character from SHADES string
    int luminance_index = static_cast<int>(luminance * static_cast<float>(SHADES.size() - 1));
    if (luminance_index < 0)
        luminance_index = 0;
    if (luminance_index >= static_cast<int>(SHADES.size()))
        luminance_index = static_cast<int>(SHADES.size() - 1);

    return SHADES[static_cast<std::size_t>(luminance_index)];
}