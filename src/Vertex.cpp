#include "Vertex.hpp"

Vertex Vertex::rotate(float cx, float sx, float cy, float sy, float cz, float sz) const
{
    Vertex out;

    // --- POSITION ROTATION ---
    float x1 = x;
    float y1 = y * cx - z * sx;
    float z1 = y * sx + z * cx;

    float x2 = x1 * cy + z1 * sy;
    float y2 = y1;
    float z2 = -x1 * sy + z1 * cy;

    out.x = x2 * cz - y2 * sz;
    out.y = x2 * sz + y2 * cz;
    out.z = z2;

    // --- NORMAL ROTATION ---
    float nx1 = nx;
    float ny1 = ny * cx - nz * sx;
    float nz1 = ny * sx + nz * cx;

    float nx2 = nx1 * cy + nz1 * sy;
    float ny2 = ny1;
    float nz2 = -nx1 * sy + nz1 * cy;

    out.nx = nx2 * cz - ny2 * sz;
    out.ny = nx2 * sz + ny2 * cz;
    out.nz = nz2;

    return out;
}