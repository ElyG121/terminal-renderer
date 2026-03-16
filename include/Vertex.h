#pragma once

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
