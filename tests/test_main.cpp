#include "test_utils.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "FixedTimestep.hpp"
#include "Framebuffer.hpp"
#include "Mat3.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "TerminalOutput.hpp"
#include "Vec3.hpp"
#include "Vertex.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <tuple>

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

    // Edge 2.0 with a point every 0.1 gives 20 steps. Six 21 x 21 face grids would be 2646 points,
    // but faces share edges; counting each shared point once gives 6 * 20^2 + 2 = 2402
    assert_eq(static_cast<int>(cube.vertices.size()), 6 * 20 * 20 + 2, "Cube should have 6 * steps^2 + 2 unique vertices");

    // No two vertices at the same position. Positions are rounded to a fine grid first,
    // because the same edge point can be computed in slightly different float ways.
    std::set<std::tuple<long, long, long>> positions;
    for (const Vertex &v : cube.vertices)
    {
        positions.insert({std::lround(v.position.x * 1000.0f), std::lround(v.position.y * 1000.0f), std::lround(v.position.z * 1000.0f)});
    }
    assert_eq(static_cast<int>(positions.size()), static_cast<int>(cube.vertices.size()), "Cube should have no duplicate vertices");

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

static void test_terminal_output_restores_cursor()
{
    std::ostringstream captured;
    {
        TerminalOutput terminal(captured);
        terminal.begin();
        assert_true(captured.str().find("\x1b[?25l") != std::string::npos, "begin() should hide the cursor");
    } // terminal is destroyed here

    assert_true(captured.str().find("\x1b[?25h") != std::string::npos, "Destructor should show the cursor again");

    // Without begin(), nothing was changed, so nothing should be restored
    std::ostringstream untouched;
    {
        TerminalOutput terminal(untouched);
    }
    assert_true(untouched.str().empty(), "Destructor should not write anything if begin() was never called");
}

static void test_fixed_timestep()
{
    // Use step sizes that floats store exactly (like 0.25), so rounding can't change the step count
    int calls = 0;
    auto count_calls = [&](float) { ++calls; };

    // The cap is raised to 2.0 here so it doesn't interfere with the step counting
    FixedTimestep ts(0.25f, 2.0f);
    assert_eq(ts.advance(1.0f, count_calls), 4, "1.0s with dt 0.25 should run 4 steps");
    assert_eq(calls, 4, "step should be called once per physics step");

    // Leftover time carries over: 0.3 runs one step and keeps 0.05, then 0.2 completes another
    FixedTimestep carry(0.25f, 2.0f);
    assert_eq(carry.advance(0.3f, count_calls), 1, "0.3s should run one step");
    assert_eq(carry.advance(0.2f, count_calls), 1, "Leftover time should carry into the next frame");

    // A long stall is capped (default 0.25s) instead of demanding 40 catch-up steps
    FixedTimestep capped(0.25f);
    assert_eq(capped.advance(10.0f, count_calls), 1, "A 10s frame should be capped to one 0.25s step");

    // A zero step would loop forever; it should do nothing instead
    FixedTimestep zero(0.0f);
    assert_eq(zero.advance(1.0f, count_calls), 0, "A zero dt should run no steps");
}

// A cube that falls under gravity and bounces, like the one in main.cpp
static Entity make_bouncing_entity()
{
    Entity e;
    e.behavior = [](Entity &self, float d)
    {
        self.motion.velocity.y += 15.0f * d;
        if (self.transform.position.y > 8.0f)
        {
            self.transform.position.y = 8.0f;
            self.motion.velocity.y = self.motion.velocity.y * -0.85f;
        }
    };
    return e;
}

static void test_fixed_timestep_is_deterministic()
{
    // The same total time (60/64 s) delivered in different frame sizes.
    // Multiples of 1/64 are exact in binary, so both runs take exactly 60 physics steps.
    const float dt = 1.0f / 64.0f;

    Entity a = make_bouncing_entity();
    FixedTimestep ts_a(dt);
    for (int i = 0; i < 15; ++i)
        ts_a.advance(4.0f * dt, [&](float step) { a.update(step); });

    Entity b = make_bouncing_entity();
    FixedTimestep ts_b(dt);
    for (int i = 0; i < 12; ++i)
        ts_b.advance(5.0f * dt, [&](float step) { b.update(step); });

    // Exactly equal, not just close: the physics never saw the frame sizes
    assert_true(a.transform.position.y == b.transform.position.y, "Fixed timestep: position should not depend on frame size");
    assert_true(a.motion.velocity.y == b.motion.velocity.y, "Fixed timestep: velocity should not depend on frame size");

    // Contrast: passing the frame time straight to update (the old approach) gives different results
    Entity c = make_bouncing_entity();
    for (int i = 0; i < 15; ++i)
        c.update(4.0f * dt);

    Entity d = make_bouncing_entity();
    for (int i = 0; i < 12; ++i)
        d.update(5.0f * dt);

    assert_true(c.transform.position.y != d.transform.position.y, "Variable timestep: position depends on frame size");
}

static void test_renderer_draw()
{
    Renderer renderer(ScreenSize{21, 11}, Camera{10.0f, 10.0f});
    renderer.set_light_direction({0.0f, 0.0f, -1.0f});

    Scene scene;
    Entity cube;
    cube.mesh = std::make_shared<const Mesh>(make_cube(1.0f));
    scene.push_back(cube);

    // One frame, returns immediately: no loop, no clock, no sleeping
    renderer.draw(scene);

    const std::vector<char> &chars = renderer.framebuffer().chars();
    assert_true(std::any_of(chars.begin(), chars.end(), [](char c) { return c != ' '; }),
                "Drawing a cube should put visible characters in the framebuffer");
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
    test_terminal_output_restores_cursor();
    test_fixed_timestep();
    test_fixed_timestep_is_deterministic();
    test_renderer_draw();

    std::cout << "All tests passed!\n";
    return 0;
}
