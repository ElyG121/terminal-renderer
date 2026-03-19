#pragma once
#include <memory>
#include "Shape.hpp"

class ShapeFactory
{
public:
    // We return unique_ptr so the caller takes ownership of the memory
    static std::unique_ptr<Shape> createCube(float size);
    static std::unique_ptr<Shape> createTorus(float R1, float R2);
};