#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h> // For usleep

int main() {
    float A = 0, B = 0;
    float i, j;
    int k;
    float z[1760];
    char b[1760];

    // Clear the terminal screen initially
    printf("\x1b[2J");

    for (;;) {
        // Reset the buffers: 32 is ASCII for space, z-buffer starts at 0
        memset(b, 32, 1760);
        memset(z, 0, 7040);

        // j sweeps from 0 to 2*pi (theta: the circle making the tube)
        for (j = 0; j < 6.28; j += 0.07) {
            // i sweeps from 0 to 2*pi (phi: the rotation around the donut center)
            for (i = 0; i < 6.28; i += 0.02) {
                
                // Precompute sines and cosines for the angles
                float sin_i = sin(i);
                float cos_j = cos(j);
                float sin_A = sin(A);
                float sin_j = sin(j);
                float cos_A = cos(A);
                float cos_i = cos(i);
                float cos_B = cos(B);
                float sin_B = sin(B);

                float h = cos_j + 2; // R1 + R2*cos(theta)
                float D = 1 / (sin_i * h * sin_A + sin_j * cos_A + 5); // 1/z (one over z)
                float t = sin_i * h * cos_A - sin_j * sin_A;

                // Project 3D coordinates (x, y) to 2D screen space
                int x = (int)(40 + 30 * D * (cos_i * h * cos_B - t * sin_B));
                int y = (int)(12 + 15 * D * (cos_i * h * sin_B + t * cos_B));
                int o = x + 80 * y;

                // Calculate Luminance (N) to determine which ASCII character to use
                // This is essentially the dot product of the surface normal and light direction
                int N = (int)(8 * ((sin_j * sin_A - sin_i * cos_j * cos_A) * cos_B 
                                   - sin_i * cos_j * sin_A - sin_j * cos_A 
                                   - cos_i * cos_j * sin_B));

                // If the point is within screen bounds and closer than what's in the z-buffer
                if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    z[o] = D;
                    // Map luminance to the character string
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0];
                }
            }
        }

        // Move cursor back to the "home" (top-left) position to redraw
        printf("\x1b[H");

        // Print the frame buffer to the terminal
        for (k = 0; k < 1761; k++) {
            putchar(k % 80 ? b[k] : 10); // Print character or newline every 80 chars
        }

        // Increment rotation angles for the next frame
        A += 0.04;
        B += 0.02;

        // Optional: slow down the animation slightly for modern CPUs
        usleep(30000); 
    }

    return 0;
}