# Terminal Renderer

A small 3D renderer written from scratch in C++17 that draws lit, animated shapes in the terminal using ASCII characters. A torus spins next to a cube that falls and bounces.

Shapes are point clouds: surface points with normals. Each frame they are rotated, projected with perspective, depth-tested with a z-buffer, and shaded by how directly each point faces the light.

## Features

- Perspective projection with near-plane clipping
- Z-buffer depth testing
- Diffuse (Lambertian) lighting mapped to the character ramp `.,-~:;=!*#$@`
- Cube and torus meshes, shareable between any number of objects
- Fixed-timestep physics (60 Hz) with per-object behaviors (e.g. gravity and bouncing)
- Clean exit on `Ctrl+C`: the cursor is restored

## Build and run

Requirements: GNU Make and `g++` with C++17 support.

```bash
make        # build bin/renderer
make run    # build and run the demo (Ctrl+C to quit)
make test   # build and run the test suite
make clean  # remove build output
```

The demo uses an 80×24 terminal and draws at about 30 FPS.

## How it works

Each pass of the main loop in `main.cpp`:

1. Measures how much time has passed since the last frame.
2. Runs physics in fixed 1/60 s steps (`FixedTimestep`). Leftover time carries over to the next frame.
3. Draws the scene into a framebuffer (`Renderer::draw`):
   1. Builds one rotation matrix per object.
   2. Rotates each point and its normal, then moves the point to the object's position.
   3. Projects the point onto the screen, keeps it only if it is the nearest so far in that cell, and picks a character from its brightness.
4. Writes the frame to the terminal (`TerminalOutput::present`).
5. Sleeps for the rest of the frame.

## Code layout

| File | Responsibility |
|---|---|
| `Vec3.hpp`, `Mat3.hpp` | Vector math and rotation matrices |
| `Vertex.hpp` | A surface point: position and normal |
| `Mesh.hpp/.cpp` | Geometry only; `make_cube()` and `make_torus()` build it |
| `Entity.hpp/.cpp` | A scene object: a shared mesh, transform, motion and optional behavior |
| `Camera.hpp` | Screen size and projection settings |
| `Framebuffer.hpp/.cpp` | Projection, z-buffer and shading into a character grid; no I/O |
| `Renderer.hpp/.cpp` | Draws one frame of a scene into its framebuffer |
| `TerminalOutput.hpp/.cpp` | Writes frames to a stream with ANSI escape codes; restores the terminal on exit |
| `FixedTimestep.hpp` | Turns uneven frame times into equal physics steps |
| `main.cpp` | Builds the scene and runs the loop |

## Design decisions

- **Composition instead of inheritance.** A scene object is an `Entity` made of a mesh, a transform, motion and an optional behavior lambda. New shapes are plain functions that return a `Mesh`. The renderer never needs to change to support them.
- **Shared, read-only geometry.** Entities hold a `std::shared_ptr<const Mesh>`, so any number of objects can reuse one mesh without copying its vertices.
- **Each class has one job.** `Framebuffer` only draws into memory, `TerminalOutput` only prints, and `Renderer` only turns a scene into a frame. `main` owns time and output, so the renderer has no loop, clock or sleeping in it, and can be tested by drawing a single frame.
- **Deterministic physics.** Physics always advances in equal steps, so the simulation doesn't depend on frame timing. A test feeds the same total time in different frame sizes and checks the results are identical.
- **Dependency injection for output.** `TerminalOutput` writes to whatever `std::ostream` it is given: `std::cout` in the program, a `std::ostringstream` in tests.

## Tests

`tests/test_main.cpp` uses a small set of assertion helpers (`tests/test_utils.hpp`) and covers:

- vector and matrix math, including a regression test for rotation order
- mesh geometry: point counts, no duplicate cube points, unit-length normals
- entity updates and mesh sharing
- projection, near-plane clipping and character aspect
- terminal output and cursor restore
- fixed-timestep stepping and determinism
- drawing a frame through `Renderer`
