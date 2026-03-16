#include "Framebuffer.h"
#include <iostream>
#include <algorithm>

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
            // --- C. LIGHTING (Luminance) ---
            // Light is coming from screen (0, 0, -1), so dot product is just -v.nz
            float luminance = -v.nz;
            if (luminance < 0)
                luminance = 0; // Clamp negative light to 0
            if (luminance > 1)
                luminance = 1; // Clamp light to 1

            // Look up character from SHADES string
            int luminance_index = (int)(luminance * 11);
            char pixel_char = SHADES[luminance_index];

            // --- D. DRAW TO BUFFERS ---
            z_buffer[index] = rot_ooz;
            buffer[index] = pixel_char;
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