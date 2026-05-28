# Terminal Renderer

A minimal ASCII-based 3D renderer for the terminal. It draws shapes with basic lighting, depth buffering, and per-vertex normals.

## Features

- ASCII rasterization with per-vertex shading
- Z-buffer depth handling
- Rotating wireframe shapes: cube and torus
- Time-based animation using `std::chrono`
- Optional frame limit support in `Renderer::render_scene`

## Build

Requirements:

- GNU Make
- `g++` with C++17 support

Build the project:

```bash
make
```

Run the renderer:

```bash
make run
```

Run the test suite:

```bash
make test
```

## Usage

The renderer continuously updates the scene at approximately 30 FPS. Use `Ctrl+C` to stop it.

## Code improvements made

- Replaced `usleep()` with `std::chrono` and `std::this_thread::sleep_for`
- Added optional frame limit support to `Renderer::render_scene`
- Fixed shading lookup to avoid out-of-bounds indexing
- Simplified terminal output line endings for Linux
- Replaced floating-point loop iteration in shape generation with integer-based steps
- Removed unnecessary header dependency from `Framebuffer.hpp`
