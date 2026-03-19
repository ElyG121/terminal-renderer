#include "Framebuffer.hpp"
#include <algorithm>
#include <cmath> // Needed for std::sqrt

void Framebuffer::set_light_direction(float lx, float ly, float lz)
{
    // Normalize the vector so the dot product stays within the [-1, 1] range
    float length = std::sqrt(lx * lx + ly * ly + lz * lz);
    if (length != 0.0f)
    {
        light_x = lx / length;
        light_y = ly / length;
        light_z = lz / length;
    }
}

void Framebuffer::clear()
{
    std::cout << "\x1b[H"; // Move cursor to start

    // Reset terminal buffers
    std::fill(buffer.begin(), buffer.end(), ' ');
    std::fill(z_buffer.begin(), z_buffer.end(), 0.0f);
}

void Framebuffer::render_vertex(const Vertex &v)
{
    // --- A. PROJECTION MATH ---
    float rot_ooz = 1.0f / (v.z + Z_OFFSET);
    int screen_x = (int)(CENTER_X + (K * v.x * 2 * rot_ooz) + 0.5f);
    int screen_y = (int)(CENTER_Y + (K * v.y * rot_ooz) + 0.5f);

    int index = screen_y * WIDTH + screen_x;

    // Bounds check
    if (screen_x >= 0 && screen_x < WIDTH && screen_y >= 0 && screen_y < HEIGHT)
    {
        // --- B. Z-BUFFER CHECK ---
        if (rot_ooz > z_buffer[index])
        {
            // --- C. DRAW TO BUFFERS ---
            z_buffer[index] = rot_ooz;
            buffer[index] = calculate_vertex_char(v);
        }
    }
}

void Framebuffer::display() const
{
    // Write to screen line-by-line
    for (int y = 0; y < HEIGHT; y++)
    {
        std::cout.write(&buffer[y * WIDTH], WIDTH);
        std::cout << "\r\n";
    }
}

char Framebuffer::calculate_vertex_char(const Vertex &v) const
{
    // Calculate the dot product of the vertex normal and the light vector
    float luminance = v.nx * light_x + v.ny * light_y + v.nz * light_z;

    if (luminance < 0)
        luminance = 0; // Clamp negative light to 0
    if (luminance > 1)
        luminance = 1; // Clamp light to 1

    // Look up character from SHADES string
    int luminance_index = (int)(luminance * 11);
    return SHADES[luminance_index];
}