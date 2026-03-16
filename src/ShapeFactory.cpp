#include "ShapeFactory.h"
#include "Cube.h"
#include "Torus.h"

std::unique_ptr<Shape> ShapeFactory::createCube(float size)
{
    // std::make_unique is the modern C++ way to safely allocate heap memory
    return std::make_unique<Cube>(size);
}

std::unique_ptr<Shape> ShapeFactory::createTorus(float R1, float R2)
{
    return std::make_unique<Torus>(R1, R2);
}