#include "test_utils.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "Framebuffer.hpp"
#include "Mat3.hpp"
#include "Mesh.hpp"
#include "TerminalOutput.hpp"
#include "Vec3.hpp"
#include "Vertex.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
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

static void test_entity_update()
{
    Entity e;
    e.motion.velocity = {1.0f, -2.0f, 3.0f};
    e.motion.angular_velocity = {0.5f, 1.0f, -0.5f};
    e.update(2.0f);

    assert_vec_near(e.transform.position, {2.0f, -4.0f, 6.0f}, "Entity position should update from velocity");
    assert_vec_near(e.transform.rotation, {1.0f, 2.0f, -1.0f}, "Entity rotation should update from angular velocity");

    // The behavior runs before the motion is applied, so a velocity change takes effect in the same update
    Entity falling;
    falling.behavior = [](Entity &self, float delta)
    { self.motion.velocity.y += 10.0f * delta; };
    falling.update(0.5f);

    assert_near(falling.motion.velocity.y, 5.0f, 1e-4f, "Behavior should change velocity");
    assert_near(falling.transform.position.y, 2.5f, 1e-4f, "Position should use the velocity set by the behavior");
}

static void test_shared_mesh()
{
    auto mesh = std::make_shared<const Mesh>(make_cube(1.0f));

    Entity a;
    a.mesh = mesh;
    Entity b;
    b.mesh = mesh;

    // One mesh, three owners: this variable plus the two entities. No vertices were copied.
    assert_eq(static_cast<int>(mesh.use_count()), 3, "Both entities should share the same mesh");
    assert_true(a.mesh.get() == b.mesh.get(), "Both entities should point to the same Mesh object");
}

static void test_meshes()
{
    Mesh cube = make_cube(2.0f);

    // Edge 2.0 with a point every 0.1 gives 20 steps: 21 x 21 points per face, 6 faces
    assert_eq(static_cast<int>(cube.vertices.size()), 21 * 21 * 6, "Cube should have (steps + 1)^2 * 6 vertices");

    for (const Vertex &v : cube.vertices)
    {
        // A point is on the surface of a cube centred at the origin when its largest coordinate is half the edge
        float largest = std::max({std::fabs(v.position.x), std::fabs(v.position.y), std::fabs(v.position.z)});
        assert_near(largest, 1.0f, 1e-4f, "Cube vertex should lie on the cube's surface");
        assert_near(length(v.normal), 1.0f, 1e-4f, "Cube normal should be unit length");
    }

    Mesh torus = make_torus(1.0f, 2.0f);
    assert_true(!torus.vertices.empty(), "Torus should have vertices");

    for (const Vertex &v : torus.vertices)
    {
        assert_near(length(v.normal), 1.0f, 1e-4f, "Torus normal should be unit length");
    }
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
    Framebuffer fb(ScreenSize{5, 5}, Camera{5.0f, 1.0f});
    fb.set_light_direction({0.0f, 0.0f, -1.0f});
    fb.clear();

    Vertex v{{0.0f, 0.0f, 10.0f}, {0.0f, 0.0f, -1.0f}};
    fb.render_vertex(v);

    // The buffer can be inspected directly, no stdout capture needed
    const std::vector<char> &chars = fb.chars();
    assert_eq(static_cast<int>(chars.size()), 5 * 5, "Framebuffer should hold width * height chars");
    assert_true(chars[2 * 5 + 2] != ' ', "Vertex at the origin should be drawn in the centre cell");
}

// Returns the column of the first visible character in the given row, or -1 if the row is empty
static int visible_column(const Framebuffer &fb, int row)
{
    for (int x = 0; x < fb.width(); ++x)
    {
        if (fb.chars()[row * fb.width() + x] != ' ')
            return x;
    }
    return -1;
}

static void test_camera_projection()
{
    // A point at the origin projects to the centre cell
    Framebuffer centre_fb(ScreenSize{5, 5}, Camera{5.0f, 1.0f});
    centre_fb.clear();
    centre_fb.render_vertex(Vertex{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}});
    assert_eq(visible_column(centre_fb, 2), 2, "Origin should land in the centre column");

    // Points at or behind the camera are clipped instead of dividing by zero or flipping
    Framebuffer clip_fb(ScreenSize{5, 5}, Camera{5.0f, 1.0f});
    clip_fb.clear();
    clip_fb.render_vertex(Vertex{{0.0f, 0.0f, -5.0f}, {0.0f, 0.0f, -1.0f}});  // exactly at the camera
    clip_fb.render_vertex(Vertex{{0.0f, 0.0f, -10.0f}, {0.0f, 0.0f, -1.0f}}); // behind the camera
    clip_fb.render_vertex(Vertex{{0.0f, 0.0f, -4.95f}, {0.0f, 0.0f, -1.0f}}); // in front, but inside the near plane
    const std::vector<char> &chars = clip_fb.chars();
    assert_true(std::all_of(chars.begin(), chars.end(), [](char c) { return c == ' '; }),
                "Points at, behind, or too close to the camera should not be drawn");

    // char_aspect stretches x: offset = focal_length * x * char_aspect / depth
    // centre column 10, focal length 10, depth 10, x = 1 -> offset of char_aspect columns
    Framebuffer wide_fb(ScreenSize{21, 5}, Camera{10.0f, 10.0f, 0.1f, 2.0f});
    wide_fb.clear();
    wide_fb.render_vertex(Vertex{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}});
    assert_eq(visible_column(wide_fb, 2), 12, "With char_aspect 2, x = 1 should be 2 columns right of centre");

    Framebuffer square_fb(ScreenSize{21, 5}, Camera{10.0f, 10.0f, 0.1f, 1.0f});
    square_fb.clear();
    square_fb.render_vertex(Vertex{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}});
    assert_eq(visible_column(square_fb, 2), 11, "With char_aspect 1, x = 1 should be 1 column right of centre");
}

static void test_terminal_output()
{
    Framebuffer fb(ScreenSize{5, 5}, Camera{5.0f, 1.0f});
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
    test_meshes();
    test_entity_update();
    test_shared_mesh();
    test_mat3_rotation();
    test_framebuffer_render();
    test_camera_projection();
    test_terminal_output();

    std::cout << "All tests passed!\n";
    return 0;
}
