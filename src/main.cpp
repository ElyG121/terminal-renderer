#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <algorithm>

struct Vertex
{
    float x, y, z;
    float nx, ny, nz;

    Vertex(float ax, float ay, float az, float anx, float any, float anz)
        : x(ax), y(ay), z(az), nx(anx), ny(any), nz(anz)
    {
    }

    Vertex() : Vertex(0, 0, 0, 0, 0, 0)
    {
    }
};

// GLOBAL RENDER SETTINGS
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 22;
const int CENTER_X = SCREEN_WIDTH / 2;
const int CENTER_Y = SCREEN_HEIGHT / 2;
const int K = 15;
const int Z_OFFSET = 25;
const std::string SHADES = ".,-~:;=!*#$@";

void render_vertex(const Vertex &v, std::vector<char> &buffer, std::vector<float> &z_buffer);
void rotate_vertex(const Vertex &v, float cos_a, float sin_a, float cos_b, float sin_b, Vertex &out);
char get_pixel_char(const Vertex &v);
std::vector<Vertex> create_cube(int size);

int main()
{
    // Define shape
    std::vector<Vertex> shape = create_cube(7);

    std::cout << "\x1b[2J"; // Clear screen

    Vertex rot_v;
    // Create the buffers ONCE outside the loops
    std::vector<char> buffer(SCREEN_WIDTH * SCREEN_HEIGHT);
    std::vector<float> z_buffer(SCREEN_WIDTH * SCREEN_HEIGHT);

    while (true)
    {
        for (float angle = 0; angle <= 2 * M_PI; angle += 0.05)
        {
            std::cout << "\x1b[H"; // Move cursor to start

            // Reset terminal buffers
            std::fill(buffer.begin(), buffer.end(), ' ');
            std::fill(z_buffer.begin(), z_buffer.end(), 0.0f);

            // Pre-calculate trig ONCE per frame
            float cos_a = cos(angle);
            float sin_a = sin(angle);
            float cos_b = cos(angle * 0.5);
            float sin_b = sin(angle * 0.5);

            for (const Vertex &v : shape)
            {
                rotate_vertex(v, cos_a, sin_a, cos_b, sin_b, rot_v);
                render_vertex(rot_v, buffer, z_buffer);
            }

            // Write to screen line-by-line
            for (int y = 0; y < SCREEN_HEIGHT; y++)
            {
                std::cout.write(&buffer[y * SCREEN_WIDTH], SCREEN_WIDTH);
                std::cout << "\r\n";
            }

            usleep(30000);
        }
    }
    return 0;
}

std::vector<Vertex> create_cube(int size)
{
    std::vector<Vertex> cube;
    int steps = (size * 2) * 10;
    cube.reserve(steps * steps * 6);

    for (float u = -size; u <= size; u += 0.1)
    {
        for (float v = -size; v <= size; v += 0.1)
        {
            cube.push_back(Vertex(u, v, size, 0, 0, 1));
            cube.push_back(Vertex(u, v, -size, 0, 0, -1));

            cube.push_back(Vertex(size, u, v, 1, 0, 0));
            cube.push_back(Vertex(-size, u, v, -1, 0, 0));

            cube.push_back(Vertex(u, size, v, 0, 1, 0));
            cube.push_back(Vertex(u, -size, v, 0, -1, 0));
        }
    }
    return cube;
}

void render_vertex(const Vertex &v, std::vector<char> &buffer, std::vector<float> &z_buffer)
{

    // Project to screen_x, screen_y
    float rot_ooz = 1.0f / (v.z + Z_OFFSET);
    int screen_x = (int)(CENTER_X + (K * v.x * 2 * rot_ooz) + 0.5f);
    int screen_y = (int)(CENTER_Y + (K * v.y * rot_ooz) + 0.5f);

    int index = screen_y * SCREEN_WIDTH + screen_x;
    if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT)
    {
        // Check Z-buffer and draw
        if (rot_ooz > z_buffer[index])
        {
            char pixel_char = get_pixel_char(v);
            z_buffer[index] = rot_ooz;
            buffer[index] = pixel_char;
        }
    }
}

void rotate_vertex(const Vertex &v, float cos_a, float sin_a, float cos_b, float sin_b, Vertex &out)
{

    // Rotate the point (x, y, z)
    out.x = v.x * cos_a - v.z * sin_a;
    out.y = -v.x * sin_a * sin_b + v.y * cos_b - v.z * cos_a * sin_b;
    out.z = v.x * sin_a * cos_b + v.y * sin_b + v.z * cos_a * cos_b;

    // Rotate the normal (nx, ny, nz)
    out.nx = v.nx * cos_a - v.nz * sin_a;
    out.ny = -v.nx * sin_a * sin_b + v.ny * cos_b - v.nz * cos_a * sin_b;
    out.nz = v.nx * sin_a * cos_b + v.ny * sin_b + v.nz * cos_a * cos_b;
}

char get_pixel_char(const Vertex &v)
{
    // Calculate luminance using the rotated normal
    // Light is coming from screen (0, 0, -1), so dot product is just -rot_nz
    float luminance = -v.nz;
    if (luminance < 0)
        luminance = 0; // Clamp negative light to 0
    if (luminance > 1)
        luminance = 1; // Clamp light to 1

    // Look up your character from ".,-~:;=!*#$@"
    int luminance_index = (int)(luminance * 11);
    return SHADES[luminance_index];
}
