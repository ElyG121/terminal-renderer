#include "test_utils.hpp"
#include "Cube.hpp"
#include "Framebuffer.hpp"
#include "Shape.hpp"
#include "ShapeFactory.hpp"
#include "Torus.hpp"
#include "Vertex.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

static void test_shape_update()
{
    Shape shape;
    shape.setPosition(0.0f, 0.0f, 0.0f);
    shape.setVelocity(1.0f, -2.0f, 3.0f);
    shape.setAngularVelocity(0.5f, 1.0f, -0.5f);
    shape.update(2.0f);

    assert_near(shape.world_x, 2.0f, 1e-4f, "Shape world_x should update from velocity");
    assert_near(shape.world_y, -4.0f, 1e-4f, "Shape world_y should update from velocity");
    assert_near(shape.world_z, 6.0f, 1e-4f, "Shape world_z should update from velocity");
    assert_near(shape.rot_x, 1.0f, 1e-4f, "Shape rot_x should update from angular velocity");
    assert_near(shape.rot_y, 2.0f, 1e-4f, "Shape rot_y should update from angular velocity");
    assert_near(shape.rot_z, -1.0f, 1e-4f, "Shape rot_z should update from angular velocity");
}

static void test_shape_factory()
{
    auto cube = ShapeFactory::createCube(2.0f);
    auto torus = ShapeFactory::createTorus(1.0f, 2.0f);

    assert_true(static_cast<bool>(cube), "createCube should return a valid unique_ptr");
    assert_true(static_cast<bool>(torus), "createTorus should return a valid unique_ptr");
    assert_true(!cube->vertices.empty(), "Cube should have vertices");
    assert_true(!torus->vertices.empty(), "Torus should have vertices");
}

static void test_vertex_rotate()
{
    Vertex input(0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    float cx = 0.0f;
    float sx = 1.0f;
    float cy = 1.0f;
    float sy = 0.0f;
    float cz = 1.0f;
    float sz = 0.0f;

    Vertex output = input.rotate(cx, sx, cy, sy, cz, sz);

    assert_near(output.x, 0.0f, 1e-4f, "Rotated vertex x should remain 0");
    assert_near(output.y, 0.0f, 1e-4f, "Rotated vertex y should become 0 after x-axis rotation");
    assert_near(output.z, 1.0f, 1e-4f, "Rotated vertex z should become 1 after x-axis rotation");
    assert_near(output.nx, 0.0f, 1e-4f, "Rotated normal nx should remain 0");
    assert_near(output.ny, 0.0f, 1e-4f, "Rotated normal ny should become 0 after x-axis rotation");
    assert_near(output.nz, 1.0f, 1e-4f, "Rotated normal nz should become 1 after x-axis rotation");
}

static void test_framebuffer_render()
{
    Framebuffer fb(5, 5, 5, 1);
    fb.set_light_direction(0.0f, 0.0f, -1.0f);
    fb.clear();

    Vertex v(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, -1.0f);
    fb.render_vertex(v);

    std::ostringstream captured;
    auto *old_buf = std::cout.rdbuf(captured.rdbuf());
    fb.display();
    std::cout.rdbuf(old_buf);

    std::string output = captured.str();
    assert_true(!output.empty(), "Framebuffer display output should not be empty");
    assert_eq(static_cast<int>(output.size()), 5 * 6, "Framebuffer output should have five rows with newlines");
    assert_true(output.find_first_not_of(" \n") != std::string::npos, "Framebuffer should render at least one visible character");
}

int main()
{
    test_shape_update();
    test_shape_factory();
    test_vertex_rotate();
    test_framebuffer_render();

    std::cout << "All tests passed!\n";
    return 0;
}
