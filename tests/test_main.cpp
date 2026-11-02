#include "test_utils.hpp"
#include "Cube.hpp"
#include "Framebuffer.hpp"
#include "Mat3.hpp"
#include "Shape.hpp"
#include "ShapeFactory.hpp"
#include "TerminalOutput.hpp"
#include "Torus.hpp"
#include "Vec3.hpp"
#include "Vertex.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>

static void assert_vec_near(const Vec3 &actual, const Vec3 &expected, const std::string &message)
{
    assert_near(actual.x, expected.x, 1e-4f, message + " (x)");
    assert_near(actual.y, expected.y, 1e-4f, message + " (y)");
    assert_near(actual.z, expected.z, 1e-4f, message + " (z)");
}

static void test_vec3()
{
    Vec3 a{1.0f, 2.0f, 3.0f};
    Vec3 b{4.0f, -5.0f, 6.0f};

    assert_vec_near(a + b, {5.0f, -3.0f, 9.0f}, "Vec3 addition");
    assert_vec_near(a - b, {-3.0f, 7.0f, -3.0f}, "Vec3 subtraction");
    assert_vec_near(a * 2.0f, {2.0f, 4.0f, 6.0f}, "Vec3 scalar multiplication");
    assert_near(dot(a, b), 12.0f, 1e-4f, "Vec3 dot product");
    assert_near(length(Vec3{3.0f, 4.0f, 0.0f}), 5.0f, 1e-4f, "Vec3 length");
    assert_vec_near(normalized(Vec3{0.0f, 0.0f, 5.0f}), {0.0f, 0.0f, 1.0f}, "Vec3 normalized");
    assert_vec_near(normalized(Vec3{}), {0.0f, 0.0f, 0.0f}, "Normalizing a zero vector should leave it unchanged");
}

static void test_shape_update()
{
    Shape shape;
    shape.setPosition({0.0f, 0.0f, 0.0f});
    shape.setVelocity({1.0f, -2.0f, 3.0f});
    shape.setAngularVelocity({0.5f, 1.0f, -0.5f});
    shape.update(2.0f);

    assert_vec_near(shape.position, {2.0f, -4.0f, 6.0f}, "Shape position should update from velocity");
    assert_vec_near(shape.rotation, {1.0f, 2.0f, -1.0f}, "Shape rotation should update from angular velocity");
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

static void test_mat3_rotation()
{
    // 90 degrees around X turns +Y into +Z
    Mat3 rx = Mat3::rotation({1.5707963f, 0.0f, 0.0f});
    assert_vec_near(rx * Vec3{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, "90 degree X rotation should turn +Y into +Z");

    // Regression: values captured from the old Vertex::rotate before it was removed,
    // so the animation is guaranteed to look the same after the refactor
    Mat3 r = Mat3::rotation({0.3f, 1.1f, -0.7f});
    assert_vec_near(r * Vec3{1.0f, 2.0f, 3.0f}, {3.363120f, -1.493731f, 0.676897f}, "Rotation should match old Vertex::rotate position");
    assert_vec_near(r * Vec3{0.6f, 0.0f, 0.8f}, {0.576805f, -0.794941f, -0.188055f}, "Rotation should match old Vertex::rotate normal");

    // Rotations preserve length, so normals stay unit length
    assert_near(length(r * Vec3{0.6f, 0.0f, 0.8f}), 1.0f, 1e-4f, "Rotation should preserve vector length");
}

static void test_framebuffer_render()
{
    Framebuffer fb(5, 5, 5, 1);
    fb.set_light_direction({0.0f, 0.0f, -1.0f});
    fb.clear();

    Vertex v{{0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, -1.0f}};
    fb.render_vertex(v);

    // The buffer can be inspected directly, no stdout capture needed
    const std::vector<char> &chars = fb.chars();
    assert_eq(static_cast<int>(chars.size()), 5 * 5, "Framebuffer should hold width * height chars");
    assert_true(chars[2 * 5 + 2] != ' ', "Vertex at the origin should be drawn in the centre cell");
}

static void test_terminal_output()
{
    Framebuffer fb(5, 5, 5, 1);
    fb.clear();
    fb.render_vertex(Vertex{{0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, -1.0f}});

    std::ostringstream captured;
    TerminalOutput terminal(captured);
    terminal.present(fb);

    std::string output = captured.str();
    assert_true(output.rfind("\x1b[H", 0) == 0, "Output should start with the cursor-home escape code");
    assert_eq(static_cast<int>(std::count(output.begin(), output.end(), '\n')), 5, "Output should have one newline per row");
    assert_eq(static_cast<int>(output.size()), 3 + 5 * 6, "Output should be the escape code plus five rows with newlines");
    assert_true(output.find_first_not_of(" \n", 3) != std::string::npos, "Output should contain at least one visible character");
}

int main()
{
    test_vec3();
    test_shape_update();
    test_shape_factory();
    test_mat3_rotation();
    test_framebuffer_render();
    test_terminal_output();

    std::cout << "All tests passed!\n";
    return 0;
}
