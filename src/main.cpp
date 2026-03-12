#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h> // For usleep

int main()
{
    const int WIDTH = 80, HEIGHT = 22;
    const int CENTER_X = WIDTH / 2, CENTER_Y = HEIGHT / 2;
    const int SIZE = 7;
    float rotation = 0.0;

    std::cout << "\x1b[2J"; // Clear once

    while (true)
    {
        std::cout << "\x1b[H";                         // Move to top EVERY frame
        std::vector<char> buffer(WIDTH * HEIGHT, ' '); // Clear buffer EVERY frame

        rotation += 0.05;

        for (float local_y = -SIZE; local_y < SIZE; local_y += 0.5)
        {
            for (float local_x = -SIZE; local_x < SIZE; local_x += 0.5)
            {

                // Rotation math
                float rot_x = local_x * cos(rotation) - local_y * sin(rotation);
                float rot_y = local_x * sin(rotation) + local_y * cos(rotation);

                int screen_x = (int)(CENTER_X + rot_x * 2);
                int screen_y = (int)(CENTER_Y + rot_y);

                if (screen_x >= 0 && screen_x < WIDTH && screen_y >= 0 && screen_y < HEIGHT)
                {
                    buffer[screen_y * WIDTH + screen_x] = '.';
                }
            }
        }

        // Print the buffer
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                std::cout << buffer[y * WIDTH + x];
            }
            std::cout << "\r\n";
        }

        usleep(30000);
    }
}