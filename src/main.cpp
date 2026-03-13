#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>

// 1. GLOBAL RENDER SETTINGS
const int SCREEN_WIDTH = 80;
const int SCREEN_HEIGHT = 22;
const int CENTER_X = SCREEN_WIDTH / 2;
const int CENTER_Y = SCREEN_HEIGHT / 2;
const int K = 15;
const int Z_OFFSET = 25;
const std::string SHADES = ".,-~:;=!*#$@";

void render_point(float x, float y, float z, float nx, float ny, float nz, float alpha, float beta, std::vector<char> &buffer, std::vector<float> &z_buffer);

int main()
{
    const int SIZE = 7;

    std::cout << "\x1b[2J"; // Clear screen

    while (true)
    {
        for (float angle = 0; angle <= 2 * M_PI; angle += 0.05)
        {
            std::cout << "\x1b[H";

            std::vector<char> buffer(SCREEN_WIDTH * SCREEN_HEIGHT, ' ');
            std::vector<float> z_buffer(SCREEN_WIDTH * SCREEN_HEIGHT, 0.0f);

            for (float u = -SIZE; u <= SIZE; u += 0.1)
            {
                for (float v = -SIZE; v <= SIZE; v += 0.1)
                {
                    // Front face (+z) -> normal points +z
                    render_point(u, v, SIZE, 0, 0, 1, angle, angle * 0.5, buffer, z_buffer);
                    // Back face (-z) -> normal points -z
                    render_point(u, v, -SIZE, 0, 0, -1, angle, angle * 0.5, buffer, z_buffer);

                    // Right face (+x) -> normal points +x
                    render_point(SIZE, u, v, 1, 0, 0, angle, angle * 0.5, buffer, z_buffer);
                    // Left face (-x) -> normal points -x
                    render_point(-SIZE, u, v, -1, 0, 0, angle, angle * 0.5, buffer, z_buffer);

                    // Top face (+y) -> normal points +y
                    render_point(u, SIZE, v, 0, 1, 0, angle, angle * 0.5, buffer, z_buffer);
                    // Bottom face (-y) -> normal points -y
                    render_point(u, -SIZE, v, 0, -1, 0, angle, angle * 0.5, buffer, z_buffer);
                }
            }

            for (int y = 0; y < SCREEN_HEIGHT; y++)
            {
                for (int x = 0; x < SCREEN_WIDTH; x++)
                {
                    std::cout << buffer[y * SCREEN_WIDTH + x];
                }
                std::cout << "\r\n";
            }

            usleep(30000);
        }
    }
    return 0;
}

void render_point(float x, float y, float z, float nx, float ny, float nz, float alpha, float beta, std::vector<char> &buffer, std::vector<float> &z_buffer)
{
    float cos_a = cos(alpha);
    float sin_a = sin(alpha);
    float cos_b = cos(beta);
    float sin_b = sin(beta);

    // 1. Rotate the point (x, y, z)
    float rot_x = x * cos_a - z * sin_a;
    float rot_y = -x * sin_a * sin_b + y * cos_b - z * cos_a * sin_b;
    float final_z = x * sin_a * cos_b + y * sin_b + z * cos_a * cos_b + Z_OFFSET;
    float rot_ooz = 1.0f / final_z;

    // 2. Rotate the normal (nx, ny, nz)
    float rot_nz = nx * sin_a * cos_b + ny * sin_b + nz * cos_a * cos_b;

    // 3. Calculate luminance using the rotated normal
    // Light is coming from screen (0, 0, -1), so dot product is just -rot_nz
    float luminance = -rot_nz;
    if (luminance < 0)
        luminance = 0; // Clamp negative light to 0

    // 4. Look up your character from ".,-~:;=!*#$@"
    int luminance_index = (int)(luminance * 11);
    char pixel_char = SHADES[luminance_index];

    // 5. Project to screen_x, screen_y
    int screen_x = (int)(CENTER_X + (K * rot_x * 2 * rot_ooz) + 0.5f);
    int screen_y = (int)(CENTER_Y + (K * rot_y * rot_ooz) + 0.5f);

    int index = screen_y * SCREEN_WIDTH + screen_x;
    if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT)
    {
        // 6. Check Z-buffer and draw
        if (rot_ooz > z_buffer[index])
        {
            z_buffer[index] = rot_ooz;
            buffer[index] = pixel_char;
        }
    }
}
