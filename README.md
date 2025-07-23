# 3D Graphics Programming Projects

This repository contains several 3D graphics projects, including classic Gouraud renderings and OpenGL-based demos.

## Requirements

- GCC (or compatible C compiler)
- Allegro libraries
- OpenGL, GLFW, GLEW (for GL projects)
- Make

## Installing Build Libraries

### On Debian-based Linux (Ubuntu, etc.)

Install the required packages using:

```sh
sudo apt update
sudo apt install build-essential liballegro5-dev libglfw3-dev libglew-dev libgl1-mesa-dev
```

### On macOS (using Homebrew)

First, install [Homebrew](https://brew.sh/) if you haven't already. Then run:

```sh
brew install allegro glew glfw
```

> **Note:** The Makefile uses `-framework OpenGL` and Homebrew include/library paths on macOS.

---

## Building the Projects

To build basic projects, simply run:

```sh
make basic
```

This will clean the `bin/` directory and build all targets.

---

## Teapot Projects

These projects use Allegro and are located in the `teapot/` and `basic/` directories.

### Build Individual Teapot Targets

- `make teapot` — Build the main teapot demo.
- `make teapot_01` to `make teapot_08` — Build teapot variants.
- `make cube` — Build the cube demo.
- `make triangle_color` — Build the colored triangle demo.
- `make test_bezier` — Build the Bezier curve test.
- `make test_triangle` — Build the triangle test.
- `make test_lines` — Build the lines test.
- `make polar_curves` — Build the polar curves demo.

Binaries are placed in the `bin/` directory.

---

## OpenGL (GL) Projects

These projects use OpenGL, GLFW, and GLEW, and are located in the `gl/` directory.

### Build All GL Projects

```sh
make gl
```

This will build all OpenGL-based demos:

- `make gl_cube` — Build the OpenGL cube demo.
- `make gl_teapot` — Build the OpenGL teapot demo.
- `make gl_teapot_diffuse` — Build the teapot with diffuse shading.
- `make gl_teapot_specular` — Build the teapot with specular shading.
- `make gl_teapot_texture` — Build the teapot with texture mapping.

Binaries are placed in the `bin/` directory.

---

## Cleaning

To remove all built binaries:

```sh
make clean
```

---

## Notes

- Ensure all required libraries are installed on your system.
- On macOS, the Makefile uses `-framework OpenGL` and Homebrew include/library paths.
- On Linux, the Makefile uses `-lGL` and standard include/library paths.

See the Makefile for further customization.


