#pragma once

// Size of the character grid to draw into
struct ScreenSize
{
    int width = 80;
    int height = 24;
};

// How 3D points are projected onto the screen
struct Camera
{
    float distance = 40.0f;     // How far the camera sits from the world origin
    float focal_length = 25.0f; // Projection scale: larger zooms in
    float near_plane = 0.1f;    // Points closer than this to the camera are not drawn
    float char_aspect = 2.0f;   // Terminal characters are about twice as tall as they are wide
};
