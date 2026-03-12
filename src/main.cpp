#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>

int main()
{
    const int SCREEN_WIDTH = 80, SCREEN_HEIGHT = 22;
    const int CENTER_X = SCREEN_WIDTH / 2, CENTER_Y = SCREEN_HEIGHT / 2;
    const int SIZE = 7;
    const int Z = 25;
    const int K = Z - 10;

    // Define our illumination palette (from dim to bright)
    const std::string shades = ".,-~:;=!*#$@";

    std::cout << "\x1b[2J";

    while (true)
    {
        for (float angle = 0; angle <= 2 * M_PI; angle += 0.05)
        {
            std::cout << "\x1b[H";

            std::vector<char> buffer(SCREEN_WIDTH * SCREEN_HEIGHT, ' ');
            std::vector<float> z_buffer(SCREEN_WIDTH * SCREEN_HEIGHT, 0.0f);

            float cos_rot = cos(angle);
            float sin_rot = sin(angle);

            // Calculate the flat surface lighting
            // Using fabs() makes it a "two-sided" square so both sides catch the light.
            float luminance = fabs(cos_rot);

            // Map the luminance (0.0 to 1.0) to an index (0 to 11)
            int luminance_index = (int)(luminance * 11);

            for (float local_y = -SIZE; local_y < SIZE; local_y += 0.1)
            {
                for (float local_x = -SIZE; local_x < SIZE; local_x += 0.1)
                {

                    // Calculate point coordinates
                    float rot_x = local_x * cos_rot;
                    float rot_z = local_x * sin_rot;
                    float rot_y = local_y;

                    float final_z = rot_z + Z;
                    float rot_ooz = 1.0f / final_z;

                    // Project to screen
                    // Added + 0.5f for cleaner rounding on the edges
                    int screen_x = (int)(CENTER_X + (K * rot_x * 2 * rot_ooz) + 0.5f);
                    int screen_y = (int)(CENTER_Y + (K * rot_y * rot_ooz) + 0.5f);

                    int index = screen_y * SCREEN_WIDTH + screen_x;
                    if (screen_x >= 0 && screen_x < SCREEN_WIDTH && screen_y >= 0 && screen_y < SCREEN_HEIGHT)
                    {
                        if (rot_ooz > z_buffer[index])
                        {
                            z_buffer[index] = rot_ooz;
                            buffer[index] = shades[luminance_index];
                        }
                    }
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
